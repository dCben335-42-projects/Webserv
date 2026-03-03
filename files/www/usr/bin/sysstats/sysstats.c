#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

static int read_file_line(const char *path, char *buf, size_t max)
{
	FILE *f = fopen(path, "r");
	if (!f)
	{
		buf[0] = '\0';
		return 0;
	}
	if (!fgets(buf, (int) max, f))
		buf[0] = '\0';
	fclose(f);
	/* strip newline */
	size_t l = strlen(buf);
	if (l && buf[l - 1] == '\n')
		buf[l - 1] = '\0';
	return 1;
}

static void get_uptime(char *out, size_t max)
{
	char buf[64];
	if (!read_file_line("/proc/uptime", buf, sizeof(buf)))
	{
		snprintf(out, max, "N/A");
		return;
	}
	long secs = (long) atof(buf);
	long d = secs / 86400, h = (secs % 86400) / 3600,
		 m = (secs % 3600) / 60, s = secs % 60;
	if (d)
		snprintf(out, max, "%ldd %ldh %ldm", d, h, m);
	else if (h)
		snprintf(out, max, "%ldh %ldm %lds", h, m, s);
	else
		snprintf(out, max, "%ldm %lds", m, s);
}

static void get_loadavg(double *a, double *b, double *c)
{
	char buf[128];
	*a = *b = *c = 0;
	if (!read_file_line("/proc/loadavg", buf, sizeof(buf)))
		return;
	sscanf(buf, "%lf %lf %lf", a, b, c);
}

typedef struct
{

		long total, free, available, buffers, cached;
} MemInfo;
static void get_meminfo(MemInfo *m)
{
	memset(m, 0, sizeof(*m));
	FILE *f = fopen("/proc/meminfo", "r");
	if (!f)
		return;
	char line[128];
	while (fgets(line, sizeof(line), f))
	{
		long v = 0;
		sscanf(line + strcspn(line, ":") + 1, "%ld", &v);
		if (!strncmp(line, "MemTotal:", 9))
			m->total = v;
		else if (!strncmp(line, "MemFree:", 8))
			m->free = v;
		else if (!strncmp(line, "MemAvailable:", 13))
			m->available = v;
		else if (!strncmp(line, "Buffers:", 8))
			m->buffers = v;
		else if (!strncmp(line, "Cached:", 7))
			m->cached = v;
	}
	fclose(f);
}

/* ── CPU model ──────────────────────────────────────────────── */
static void get_cpu_model(char *out, size_t max)
{
	FILE *f = fopen("/proc/cpuinfo", "r");
	if (!f)
	{
		snprintf(out, max, "Unknown");
		return;
	}
	char line[256];
	while (fgets(line, sizeof(line), f))
	{
		if (!strncmp(line, "model name", 10))
		{
			char *p = strchr(line, ':');
			if (p)
			{
				p += 2;
				size_t l = strlen(p);
				if (l && p[l - 1] == '\n')
					p[l - 1] = '\0';
				snprintf(out, max, "%s", p);
				fclose(f);
				return;
			}
		}
	}
	fclose(f);
	snprintf(out, max, "Unknown");
}

/* ── CPU usage via two /proc/stat reads ─────────────────────── */
typedef struct
{
		long long user, nice, sys, idle, iowait, irq, softirq;
} CpuStat;
static void read_cpu_stat(CpuStat *s)
{
	memset(s, 0, sizeof(*s));
	FILE *f = fopen("/proc/stat", "r");
	if (!f)
		return;
	char line[256];
	if (fgets(line, sizeof(line), f))
		sscanf(line, "cpu %lld %lld %lld %lld %lld %lld %lld",
			   &s->user, &s->nice, &s->sys, &s->idle, &s->iowait, &s->irq, &s->softirq);
	fclose(f);
}
static double get_cpu_usage(void)
{
	CpuStat a, b;
	read_cpu_stat(&a);
	usleep(200000); /* 200ms sample */
	read_cpu_stat(&b);
	long long idle_d  = (b.idle + b.iowait) - (a.idle + a.iowait);
	long long total_d = (b.user + b.nice + b.sys + b.idle + b.iowait + b.irq + b.softirq) - (a.user + a.nice + a.sys + a.idle + a.iowait + a.irq + a.softirq);
	if (total_d <= 0)
		return 0;
	return 100.0 * (1.0 - (double) idle_d / total_d);
}

/* ── Disk usage ─────────────────────────────────────────────── */
typedef struct
{
		long long total, used, avail;
} DiskInfo;
static void get_disk(DiskInfo *d)
{
	memset(d, 0, sizeof(*d));
	FILE *p = popen("df -k / 2>/dev/null | tail -1", "r");
	if (!p)
		return;
	char line[256] = "";
	fgets(line, sizeof(line), p);
	pclose(p);
	long long t = 0, u = 0, a = 0;
	sscanf(line, "%*s %lld %lld %lld", &t, &u, &a);
	d->total = t;
	d->used	 = u;
	d->avail = a;
}

/* ── CPU core count ─────────────────────────────────────────── */
static int get_cpu_count(void)
{
	int	  n = 0;
	FILE *f = fopen("/proc/cpuinfo", "r");
	if (!f)
		return 1;
	char line[128];
	while (fgets(line, sizeof(line), f))
		if (!strncmp(line, "processor", 9))
			n++;
	fclose(f);
	return n ? n : 1;
}

/* ══════════════════════════════════════════════════════════════
   RENDER
   ══════════════════════════════════════════════════════════════ */
static void bar(double pct, const char *color)
{
	if (pct < 0)
		pct = 0;
	if (pct > 100)
		pct = 100;
	printf(
		"<div style='background:#111;border-radius:2px;height:8px;overflow:hidden;margin-top:6px'>"
		"<div style='height:100%%;width:%.1f%%;background:%s;border-radius:2px;"
		"transition:width .6s ease'></div></div>\n",
		pct, color);
}

int main(void)
{
	/* gather data first (cpu sample takes 200ms) */
	char	 uptime[64], cpu_model[256];
	double	 la1, la2, la5;
	MemInfo	 mem;
	DiskInfo disk;

	get_uptime(uptime, sizeof(uptime));
	get_loadavg(&la1, &la2, &la5);
	get_meminfo(&mem);
	get_cpu_model(cpu_model, sizeof(cpu_model));
	double cpu_pct = get_cpu_usage();
	get_disk(&disk);
	int ncpu = get_cpu_count();

	double mem_pct	= mem.total ? 100.0 * (mem.total - mem.available) / mem.total : 0;
	double disk_pct = disk.total ? 100.0 * disk.used / disk.total : 0;

	time_t	   now = time(NULL);
	struct tm *t   = gmtime(&now);
	char	   ts[64];
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M:%S UTC", t);

	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
	printf(
		"<!DOCTYPE html><html lang='en'><head>\n"
		"<meta charset='UTF-8'>\n"
		"<meta name='viewport' content='width=device-width,initial-scale=1'>\n"
		"<meta http-equiv='refresh' content='5'>\n"
		"<title>sysmon</title>\n"
		"<link href='https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;600&display=swap' rel='stylesheet'>\n"
		"<style>\n"
		"*{box-sizing:border-box;margin:0;padding:0}\n"
		"body{background:#050508;color:#a0ffa0;font-family:'JetBrains Mono',monospace;"
		"     font-size:13px;min-height:100vh;padding:2rem;"
		"     background-image:radial-gradient(ellipse 60%% 50%% at 50%% 0%%,rgba(0,255,80,.04) 0%%,transparent 70%%);}\n"
		"h1{font-size:1.1rem;font-weight:600;letter-spacing:.2em;color:#00ff50;"
		"   text-shadow:0 0 20px rgba(0,255,80,.5);margin-bottom:.3rem}\n"
		".ts{font-size:.7rem;color:#3a5a3a;letter-spacing:.12em;margin-bottom:2rem}\n"
		".grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(280px,1fr));gap:1rem}\n"
		".card{background:#0a0f0a;border:1px solid #1a2f1a;border-radius:3px;padding:1.4rem;\n"
		"      position:relative;overflow:hidden}\n"
		".card::before{content:'';position:absolute;top:0;left:0;right:0;height:1px;\n"
		"  background:linear-gradient(90deg,transparent,#00ff50,transparent);opacity:.3}\n"
		".card-title{font-size:.65rem;letter-spacing:.2em;color:#3a7a3a;text-transform:uppercase;margin-bottom:1rem}\n"
		".val{font-size:1.8rem;font-weight:600;color:#00ff50;text-shadow:0 0 15px rgba(0,255,80,.4);line-height:1}\n"
		".val-unit{font-size:.75rem;color:#3a7a3a;margin-left:4px}\n"
		".sub{font-size:.72rem;color:#3a6a3a;margin-top:.4rem}\n"
		".row{display:flex;justify-content:space-between;align-items:baseline;margin-bottom:.5rem}\n"
		".label{color:#3a6a3a;font-size:.7rem}\n"
		".number{color:#a0ffa0}\n"
		".cpu-model{font-size:.68rem;color:#2a5a2a;margin-top:.6rem;white-space:nowrap;"
		"           overflow:hidden;text-overflow:ellipsis}\n"
		".warn{color:#ffaa00;text-shadow:0 0 10px rgba(255,170,0,.4)}\n"
		".crit{color:#ff4040;text-shadow:0 0 10px rgba(255,60,0,.4)}\n"
		".blink{animation:blink 1s step-start infinite}\n"
		"@keyframes blink{50%%{opacity:0}}\n"
		".refresh{font-size:.6rem;color:#1a3a1a;margin-top:2rem;letter-spacing:.1em}\n"
		".refresh span{color:#2a6a2a}\n"
		"</style></head><body>\n");

	printf("<h1>[ sysmon ]</h1>\n");
	printf("<p class='ts'>%s &nbsp;·&nbsp; refreshes every 5s</p>\n", ts);
	printf("<div class='grid'>\n");

	/* ── CPU card ── */
	const char *cpu_color = cpu_pct > 85 ? "#ff4040" : cpu_pct > 60 ? "#ffaa00"
																	: "#00ff50";
	printf("<div class='card'>\n");
	printf("<p class='card-title'>CPU Usage</p>\n");
	printf("<div><span class='val %s'>%.1f</span><span class='val-unit'>%%</span></div>\n",
		   cpu_pct > 85 ? "crit" : cpu_pct > 60 ? "warn"
												: "",
		   cpu_pct);
	bar(cpu_pct, cpu_color);
	printf("<p class='cpu-model' title='%s'>%s</p>\n", cpu_model, cpu_model);
	printf("<p class='sub'>%d core%s</p>\n", ncpu, ncpu > 1 ? "s" : "");
	printf("</div>\n");

	/* ── Load avg card ── */
	printf("<div class='card'>\n");
	printf("<p class='card-title'>Load Average</p>\n");
	printf("<div><span class='val'>%.2f</span><span class='val-unit'>1m</span></div>\n", la1);
	printf("<div class='row' style='margin-top:.8rem'>\n");
	printf("  <span class='label'>5 min</span><span class='number'>%.2f</span>\n", la2);
	printf("</div><div class='row'>\n");
	printf("  <span class='label'>15 min</span><span class='number'>%.2f</span>\n", la5);
	printf("</div>\n");
	printf("<p class='sub'>threshold: %d.00</p>\n", ncpu);
	printf("</div>\n");

	/* ── Memory card ── */
	long		used_mb	  = (mem.total - mem.available) / 1024;
	long		total_mb  = mem.total / 1024;
	long		avail_mb  = mem.available / 1024;
	const char *mem_color = mem_pct > 90 ? "#ff4040" : mem_pct > 70 ? "#ffaa00"
																	: "#00ff50";
	printf("<div class='card'>\n");
	printf("<p class='card-title'>Memory</p>\n");
	printf("<div><span class='val %s'>%ld</span><span class='val-unit'>MB used</span></div>\n",
		   mem_pct > 90 ? "crit" : mem_pct > 70 ? "warn"
												: "",
		   used_mb);
	bar(mem_pct, mem_color);
	printf("<div class='row' style='margin-top:.8rem'>\n");
	printf("  <span class='label'>total</span><span class='number'>%ld MB</span>\n", total_mb);
	printf("</div><div class='row'>\n");
	printf("  <span class='label'>available</span><span class='number'>%ld MB</span>\n", avail_mb);
	printf("</div>\n");
	printf("</div>\n");

	/* ── Disk card ── */
	long long	used_gb	   = disk.used / 1024 / 1024;
	long long	total_gb   = disk.total / 1024 / 1024;
	long long	avail_gb   = disk.avail / 1024 / 1024;
	const char *disk_color = disk_pct > 90 ? "#ff4040" : disk_pct > 75 ? "#ffaa00"
																	   : "#00ff50";
	printf("<div class='card'>\n");
	printf("<p class='card-title'>Disk  /</p>\n");
	printf("<div><span class='val %s'>%.0f</span><span class='val-unit'>%% used</span></div>\n",
		   disk_pct > 90 ? "crit" : disk_pct > 75 ? "warn"
												  : "",
		   disk_pct);
	bar(disk_pct, disk_color);
	printf("<div class='row' style='margin-top:.8rem'>\n");
	printf("  <span class='label'>used</span><span class='number'>%lld GB</span>\n", used_gb);
	printf("</div><div class='row'>\n");
	printf("  <span class='label'>total</span><span class='number'>%lld GB</span>\n", total_gb);
	printf("</div><div class='row'>\n");
	printf("  <span class='label'>free</span><span class='number'>%lld GB</span>\n", avail_gb);
	printf("</div>\n");
	printf("</div>\n");

	/* ── Uptime card ── */
	printf("<div class='card'>\n");
	printf("<p class='card-title'>Uptime</p>\n");
	printf("<div class='val' style='font-size:1.3rem'>%s</div>\n", uptime);
	printf("</div>\n");

	printf("</div>\n"); /* grid */
	printf("<p class='refresh'><span class='blink'>■</span> auto-refresh every <span>5s</span></p>\n");
	printf("</body></html>\n");
	return 0;
}