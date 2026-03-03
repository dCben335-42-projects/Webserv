#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <sys/stat.h>

#define MAX_NAME 128
#define MAX_MSG 1024
#define MAX_COLOR 16
#define DATA_FILE "guestbook_entries.txt"
#define MAX_ENTRIES 200
#define FIELD_SEP "\x01"
#define ENTRY_SEP "\x02"

/* ── URL decode ─────────────────────────────────────────────── */
static int hex2int(char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'a' && c <= 'f')
		return c - 'a' + 10;
	if (c >= 'A' && c <= 'F')
		return c - 'A' + 10;
	return 0;
}

static void url_decode(const char *src, char *dst, size_t max)
{
	size_t i = 0;
	while (*src && i < max - 1)
	{
		if (*src == '%' && src[1] && src[2])
		{
			dst[i++] = (char) (hex2int(src[1]) * 16 + hex2int(src[2]));
			src += 3;
		}
		else if (*src == '+')
		{
			dst[i++] = ' ';
			src++;
		}
		else
		{
			dst[i++] = *src++;
		}
	}
	dst[i] = '\0';
}

/* ── HTML escape ─────────────────────────────────────────────── */
static void html_escape(const char *src, char *dst, size_t max)
{
	size_t i = 0;
	while (*src && i < max - 7)
	{
		switch (*src)
		{
			case '&':
				strcpy(dst + i, "&amp;");
				i += 5;
				break;
			case '<':
				strcpy(dst + i, "&lt;");
				i += 4;
				break;
			case '>':
				strcpy(dst + i, "&gt;");
				i += 4;
				break;
			case '"':
				strcpy(dst + i, "&quot;");
				i += 6;
				break;
			case '\'':
				strcpy(dst + i, "&#39;");
				i += 5;
				break;
			default:
				dst[i++] = *src;
		}
		src++;
	}
	dst[i] = '\0';
}

/* ── Parse query/body key=value pairs ───────────────────────── */
static char *get_param(const char *query, const char *key)
{
	if (!query)
		return NULL;
	size_t		klen = strlen(key);
	const char *p	 = query;
	while (*p)
	{
		if (strncmp(p, key, klen) == 0 && p[klen] == '=')
		{
			const char *val	 = p + klen + 1;
			const char *end	 = strchr(val, '&');
			size_t		vlen = end ? (size_t) (end - val) : strlen(val);
			char	   *raw	 = (char *) malloc(vlen + 1);
			strncpy(raw, val, vlen);
			raw[vlen]	  = '\0';
			char *decoded = (char *) malloc(vlen * 3 + 1);
			url_decode(raw, decoded, vlen * 3 + 1);
			free(raw);
			return decoded;
		}
		p = strchr(p, '&');
		if (!p)
			break;
		p++;
	}
	return NULL;
}

/* ── Sanitize: strip control chars except space ─────────────── */
static void sanitize(char *s)
{
	for (char *p = s; *p; p++)
		if ((unsigned char) *p < 0x20 && *p != '\t')
			*p = ' ';
}

/* ── Validate color choice ───────────────────────────────────── */
static const char *valid_colors[] = {
	"crimson", "coral", "gold", "lime", "cyan", "violet", "white", "orange", NULL};
static int is_valid_color(const char *c)
{
	for (int i = 0; valid_colors[i]; i++)
		if (strcmp(c, valid_colors[i]) == 0)
			return 1;
	return 0;
}

/* ── Save entry ──────────────────────────────────────────────── */
static void save_entry(const char *name, const char *msg, const char *color)
{
	time_t	   now = time(NULL);
	struct tm *t   = gmtime(&now);
	char	   ts[32];
	strftime(ts, sizeof(ts), "%Y-%m-%d %H:%M UTC", t);

	FILE *f = fopen(DATA_FILE, "a");
	if (!f)
		return;
	/* format: name\x01message\x01color\x01timestamp\x02 */
	fprintf(f, "%s\x01%s\x01%s\x01%s\x02", name, msg, color, ts);
	fclose(f);
}

typedef struct
{
		char name[MAX_NAME];
		char msg[MAX_MSG];
		char color[MAX_COLOR];
		char ts[32];
} Entry;

/* ── Load entries ────────────────────────────────────────────── */
static int load_entries(Entry *entries, int max)
{
	FILE *f = fopen(DATA_FILE, "r");
	if (!f)
		return 0;

	/* read entire file */
	fseek(f, 0, SEEK_END);
	long sz = ftell(f);
	rewind(f);
	if (sz <= 0)
	{
		fclose(f);
		return 0;
	}
	char *buf = (char *) malloc(sz + 1);
	fread(buf, 1, sz, f);
	buf[sz] = '\0';
	fclose(f);

	int	  count = 0;
	char *entry = buf;
	while (*entry && count < max)
	{
		char *end = strchr(entry, '\x02');
		if (!end)
			break;
		*end = '\0';

		char *f1 = strtok(entry, "\x01");
		char *f2 = strtok(NULL, "\x01");
		char *f3 = strtok(NULL, "\x01");
		char *f4 = strtok(NULL, "\x01");

		if (f1 && f2 && f3 && f4)
		{
			strncpy(entries[count].name, f1, MAX_NAME - 1);
			strncpy(entries[count].msg, f2, MAX_MSG - 1);
			strncpy(entries[count].color, f3, MAX_COLOR - 1);
			strncpy(entries[count].ts, f4, 31);
			count++;
		}
		entry = end + 1;
	}
	free(buf);
	return count;
}

/* ══════════════════════════════════════════════════════════════
   HTML OUTPUT
   ══════════════════════════════════════════════════════════════ */
static void print_header(void)
{
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
	printf(
		"<!DOCTYPE html><html lang='en'><head>\n"
		"<meta charset='UTF-8'>\n"
		"<meta name='viewport' content='width=device-width, initial-scale=1'>\n"
		"<title>✦ The Guestbook ✦</title>\n"
		"<link href='https://fonts.googleapis.com/css2?family=Playfair+Display:ital,wght@0,400;0,700;1,400&family=DM+Mono:wght@400;500&display=swap' rel='stylesheet'>\n"
		"<style>\n"
		":root{\n"
		"  --bg:#0a0a0f;\n"
		"  --surface:#12121a;\n"
		"  --border:#2a2a3a;\n"
		"  --text:#e8e4d9;\n"
		"  --muted:#6b6880;\n"
		"  --accent:#c9a84c;\n"
		"  --accent2:#8b5cf6;\n"
		"}\n"
		"*{box-sizing:border-box;margin:0;padding:0}\n"
		"body{\n"
		"  background:var(--bg);\n"
		"  color:var(--text);\n"
		"  font-family:'DM Mono',monospace;\n"
		"  min-height:100vh;\n"
		"  padding:2rem 1rem 4rem;\n"
		"  background-image:\n"
		"    radial-gradient(ellipse 80%% 40%% at 50%% -10%%, rgba(139,92,246,0.15) 0%%, transparent 60%%),\n"
		"    radial-gradient(ellipse 60%% 30%% at 80%% 90%%, rgba(201,168,76,0.08) 0%%, transparent 50%%);\n"
		"}\n"
		".container{max-width:760px;margin:0 auto}\n"

		/* ── header ── */
		".site-header{text-align:center;padding:3rem 0 2.5rem;position:relative}\n"
		".site-header::after{content:'';display:block;height:1px;\n"
		"  background:linear-gradient(90deg,transparent,var(--accent),transparent);\n"
		"  margin-top:2rem}\n"
		".eyebrow{font-size:.65rem;letter-spacing:.3em;color:var(--accent);text-transform:uppercase;margin-bottom:1rem}\n"
		"h1{font-family:'Playfair Display',serif;font-size:clamp(2.4rem,6vw,3.8rem);\n"
		"   font-weight:700;line-height:1.05;\n"
		"   background:linear-gradient(135deg,#e8e4d9 30%%,#c9a84c 60%%,#8b5cf6);\n"
		"   -webkit-background-clip:text;-webkit-text-fill-color:transparent;background-clip:text}\n"
		".subtitle{margin-top:.8rem;font-size:.78rem;color:var(--muted);letter-spacing:.08em}\n"

		/* ── form ── */
		".form-card{\n"
		"  background:var(--surface);\n"
		"  border:1px solid var(--border);\n"
		"  border-radius:4px;\n"
		"  padding:2rem;\n"
		"  margin:2.5rem 0;\n"
		"  position:relative;\n"
		"  overflow:hidden;\n"
		"}\n"
		".form-card::before{\n"
		"  content:'';\n"
		"  position:absolute;top:0;left:0;right:0;height:2px;\n"
		"  background:linear-gradient(90deg,var(--accent2),var(--accent));\n"
		"}\n"
		".form-title{font-size:.7rem;letter-spacing:.2em;color:var(--accent);text-transform:uppercase;margin-bottom:1.5rem}\n"
		".field{margin-bottom:1.2rem}\n"
		"label{display:block;font-size:.68rem;letter-spacing:.12em;color:var(--muted);text-transform:uppercase;margin-bottom:.5rem}\n"
		"input[type=text],textarea{\n"
		"  width:100%%;\n"
		"  background:#0d0d15;\n"
		"  border:1px solid var(--border);\n"
		"  border-radius:3px;\n"
		"  color:var(--text);\n"
		"  font-family:'DM Mono',monospace;\n"
		"  font-size:.85rem;\n"
		"  padding:.7rem 1rem;\n"
		"  transition:border-color .2s;\n"
		"  resize:vertical;\n"
		"}\n"
		"input[type=text]:focus,textarea:focus{\n"
		"  outline:none;border-color:var(--accent);box-shadow:0 0 0 3px rgba(201,168,76,.1)\n"
		"}\n"
		"textarea{min-height:90px}\n"
		".color-row{display:flex;gap:.5rem;flex-wrap:wrap}\n"
		".color-opt{cursor:pointer}\n"
		".color-opt input{display:none}\n"
		".color-swatch{\n"
		"  width:28px;height:28px;border-radius:50%%;\n"
		"  border:2px solid transparent;\n"
		"  transition:transform .15s,border-color .15s;\n"
		"  display:block;\n"
		"}\n"
		".color-opt input:checked + .color-swatch{\n"
		"  border-color:var(--text);\n"
		"  transform:scale(1.2);\n"
		"  box-shadow:0 0 8px currentColor;\n"
		"}\n"
		"button[type=submit]{\n"
		"  margin-top:1.2rem;\n"
		"  background:linear-gradient(135deg,var(--accent2),var(--accent));\n"
		"  color:#0a0a0f;\n"
		"  font-family:'DM Mono',monospace;\n"
		"  font-size:.8rem;\n"
		"  font-weight:500;\n"
		"  letter-spacing:.15em;\n"
		"  text-transform:uppercase;\n"
		"  border:none;\n"
		"  padding:.75rem 2rem;\n"
		"  border-radius:3px;\n"
		"  cursor:pointer;\n"
		"  transition:opacity .2s,transform .1s;\n"
		"}\n"
		"button[type=submit]:hover{opacity:.88}\n"
		"button[type=submit]:active{transform:scale(.97)}\n"

		/* ── entries ── */
		".entries-header{\n"
		"  display:flex;align-items:center;gap:1rem;\n"
		"  margin-bottom:1.5rem\n"
		"}\n"
		".entries-label{font-size:.68rem;letter-spacing:.2em;color:var(--muted);text-transform:uppercase}\n"
		".entries-count{\n"
		"  font-family:'Playfair Display',serif;\n"
		"  font-size:1.1rem;color:var(--accent)\n"
		"}\n"
		".divider{flex:1;height:1px;background:var(--border)}\n"
		".entry{\n"
		"  border:1px solid var(--border);\n"
		"  border-left:3px solid;\n"
		"  border-radius:0 4px 4px 0;\n"
		"  background:var(--surface);\n"
		"  padding:1.2rem 1.4rem;\n"
		"  margin-bottom:1rem;\n"
		"  animation:fadeUp .4s ease both;\n"
		"}\n"
		"@keyframes fadeUp{\n"
		"  from{opacity:0;transform:translateY(12px)}\n"
		"  to{opacity:1;transform:translateY(0)}\n"
		"}\n"
		".entry-meta{display:flex;justify-content:space-between;align-items:baseline;margin-bottom:.6rem}\n"
		".entry-name{font-family:'Playfair Display',serif;font-size:1.05rem;font-weight:700}\n"
		".entry-ts{font-size:.65rem;color:var(--muted);letter-spacing:.06em}\n"
		".entry-msg{font-size:.85rem;line-height:1.65;color:#ccc7bb;white-space:pre-wrap;word-break:break-word}\n"

		/* ── empty state ── */
		".empty{text-align:center;padding:3rem;color:var(--muted);font-size:.8rem;letter-spacing:.1em}\n"
		".empty span{display:block;font-size:2rem;margin-bottom:.8rem;opacity:.3}\n"

		/* ── flash ── */
		".flash{\n"
		"  background:rgba(201,168,76,.1);\n"
		"  border:1px solid rgba(201,168,76,.3);\n"
		"  border-radius:3px;\n"
		"  padding:.8rem 1.2rem;\n"
		"  font-size:.78rem;\n"
		"  color:var(--accent);\n"
		"  margin-bottom:1.5rem;\n"
		"  letter-spacing:.06em;\n"
		"}\n"
		".footer{text-align:center;margin-top:4rem;font-size:.62rem;color:var(--muted);letter-spacing:.15em}\n"
		".footer a{color:var(--muted);text-decoration:none;border-bottom:1px solid var(--border)}\n"
		"</style></head><body>\n"
		"<div class='container'>\n"
		"<header class='site-header'>\n"
		"  <p class='eyebrow'>✦ since the dawn of the web ✦</p>\n"
		"  <h1>The Guestbook</h1>\n"
		"  <p class='subtitle'>leave your mark — the internet remembers</p>\n"
		"</header>\n");
}

static void print_form(const char *error)
{
	if (error && *error)
	{
		char esc[512];
		html_escape(error, esc, sizeof(esc));
		printf("<div class='flash'>⚠ %s</div>\n", esc);
	}
	printf(
		"<div class='form-card'>\n"
		"<p class='form-title'>Sign the book</p>\n"
		"<form method='POST' action=''>\n"
		"  <div class='field'>\n"
		"    <label for='name'>Your name</label>\n"
		"    <input type='text' id='name' name='name' maxlength='64' placeholder='Anonymous Wanderer' required>\n"
		"  </div>\n"
		"  <div class='field'>\n"
		"    <label for='msg'>Your message</label>\n"
		"    <textarea id='msg' name='msg' maxlength='500' placeholder='Write something memorable...' required></textarea>\n"
		"  </div>\n"
		"  <div class='field'>\n"
		"    <label>Pick your color</label>\n"
		"    <div class='color-row'>\n");

	struct
	{
			const char *name;
			const char *hex;
	} colors[] = {
		{"crimson", "#dc143c"}, {"coral", "#ff6b6b"}, {"gold", "#c9a84c"}, {"lime", "#4ade80"}, {"cyan", "#22d3ee"}, {"violet", "#8b5cf6"}, {"white", "#e8e4d9"}, {"orange", "#fb923c"}, {NULL, NULL}};
	for (int i = 0; colors[i].name; i++)
	{
		printf(
			"      <label class='color-opt'>\n"
			"        <input type='radio' name='color' value='%s'%s>\n"
			"        <span class='color-swatch' style='background:%s' title='%s'></span>\n"
			"      </label>\n",
			colors[i].name, i == 2 ? " checked" : "", colors[i].hex, colors[i].name);
	}
	printf(
		"    </div>\n"
		"  </div>\n"
		"  <button type='submit'>✦ Sign &amp; Publish</button>\n"
		"</form>\n"
		"</div>\n");
}

static void print_entries(Entry *entries, int count)
{
	printf("<div class='entries-header'>\n");
	printf("  <span class='entries-count'>%d</span>\n", count);
	printf("  <span class='entries-label'>%s</span>\n", count == 1 ? "entry" : "entries");
	printf("  <span class='divider'></span>\n");
	printf("</div>\n");

	if (count == 0)
	{
		printf("<div class='empty'><span>✦</span>No entries yet — be the first!</div>\n");
		return;
	}

	/* print newest first */
	char ne[MAX_NAME * 2], me[MAX_MSG * 3], te[64];
	for (int i = count - 1; i >= 0; i--)
	{
		html_escape(entries[i].name, ne, sizeof(ne));
		html_escape(entries[i].msg, me, sizeof(me));
		html_escape(entries[i].ts, te, sizeof(te));
		const char *color = is_valid_color(entries[i].color) ? entries[i].color : "gold";

		/* stagger animation */
		int delay_ms = (count - 1 - i) * 60;

		printf(
			"<div class='entry' style='border-left-color:%s;animation-delay:%dms'>\n"
			"  <div class='entry-meta'>\n"
			"    <span class='entry-name' style='color:%s'>%s</span>\n"
			"    <span class='entry-ts'>%s</span>\n"
			"  </div>\n"
			"  <p class='entry-msg'>%s</p>\n"
			"</div>\n",
			color, delay_ms, color, ne, te, me);
	}
}

static void print_footer(void)
{
	printf(
		"<footer class='footer'>\n"
		"  <p>powered by C &amp; raw CGI &nbsp;·&nbsp; no js required &nbsp;·&nbsp; no frameworks harmed</p>\n"
		"</footer>\n"
		"</div></body></html>\n");
}

/* ══════════════════════════════════════════════════════════════
   MAIN
   ══════════════════════════════════════════════════════════════ */
int main(void)
{
	const char *method = getenv("REQUEST_METHOD");
	const char *qs	   = getenv("QUERY_STRING");
	const char *clen_s = getenv("CONTENT_LENGTH");

	char *body		 = NULL;
	char  error[256] = "";

	/* ── Read POST body ───────────────────────────────────────── */
	if (method && strcmp(method, "POST") == 0 && clen_s)
	{
		int clen = atoi(clen_s);
		if (clen > 0 && clen < 8192)
		{
			body	= (char *) malloc(clen + 1);
			int n	= (int) fread(body, 1, clen, stdin);
			body[n] = '\0';
		}
	}

	/* ── Handle POST ─────────────────────────────────────────── */
	if (body)
	{
		char *name_raw	= get_param(body, "name");
		char *msg_raw	= get_param(body, "msg");
		char *color_raw = get_param(body, "color");

		char name[MAX_NAME]	  = "Anonymous";
		char msg[MAX_MSG]	  = "";
		char color[MAX_COLOR] = "gold";

		if (name_raw)
		{
			strncpy(name, name_raw, MAX_NAME - 1);
			sanitize(name);
			free(name_raw);
		}
		if (msg_raw)
		{
			strncpy(msg, msg_raw, MAX_MSG - 1);
			sanitize(msg);
			free(msg_raw);
		}
		if (color_raw)
		{
			strncpy(color, color_raw, MAX_COLOR - 1);
			free(color_raw);
		}

		/* validate */
		if (strlen(name) < 1)
			strncpy(error, "Name cannot be empty.", sizeof(error) - 1);
		else if (strlen(msg) < 1)
			strncpy(error, "Message cannot be empty.", sizeof(error) - 1);
		else if (!is_valid_color(color))
			strncpy(color, "gold", sizeof(color) - 1);

		if (!*error)
		{
			save_entry(name, msg, color);
			free(body);
			static Entry entries[MAX_ENTRIES];
			int			 count = load_entries(entries, MAX_ENTRIES);
			print_header();
			printf("<div class='flash' style='border-color:rgba(74,222,128,.3);background:rgba(74,222,128,.08);color:#4ade80'>✦ Entry saved — welcome to the book!</div>\n");
			print_form("");
			print_entries(entries, count);
			print_footer();
			return 0;
		}
		free(body);
	}

	static Entry entries[MAX_ENTRIES];
	int			 count = load_entries(entries, MAX_ENTRIES);

	print_header();
	print_form(error);
	print_entries(entries, count);
	print_footer();
	return 0;
}