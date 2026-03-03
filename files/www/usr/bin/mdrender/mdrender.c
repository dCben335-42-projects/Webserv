#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_BODY (256 * 1024)

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
			dst[i++] = *src++;
	}
	dst[i] = '\0';
}
static char *get_param(const char *query, const char *key)
{
	if (!query)
		return NULL;
	size_t		klen = strlen(key);
	const char *p	 = query;
	while (*p)
	{
		if (!strncmp(p, key, klen) && p[klen] == '=')
		{
			const char *v	 = p + klen + 1;
			const char *e	 = strchr(v, '&');
			size_t		vlen = e ? (size_t) (e - v) : strlen(v);
			char	   *raw	 = (char *) malloc(vlen + 1);
			strncpy(raw, v, vlen);
			raw[vlen] = '\0';
			char *dec = (char *) malloc(vlen * 3 + 1);
			url_decode(raw, dec, vlen * 3 + 1);
			free(raw);
			return dec;
		}
		p = strchr(p, '&');
		if (!p)
			break;
		p++;
	}
	return NULL;
}
static void he(const char *s, int len)
{
	for (int i = 0; i < len && s[i]; i++)
	{
		switch (s[i])
		{
			case '&':
				printf("&amp;");
				break;
			case '<':
				printf("&lt;");
				break;
			case '>':
				printf("&gt;");
				break;
			case '"':
				printf("&quot;");
				break;
			default:
				putchar(s[i]);
		}
	}
}
static void hes(const char *s)
{
	he(s, (int) strlen(s));
}

static void render_inline(const char *s, int len)
{
	int i = 0;
	while (i < len)
	{
		if (s[i] == '`')
		{
			int j = i + 1;
			while (j < len && s[j] != '`')
				j++;
			printf("<code>");
			he(s + i + 1, j - i - 1);
			printf("</code>");
			i = j + (j < len ? 1 : 0);
			continue;
		}
		if (i + 2 < len && s[i] == '*' && s[i + 1] == '*' && s[i + 2] == '*')
		{
			int j = i + 3;
			while (j + 2 < len && !(s[j] == '*' && s[j + 1] == '*' && s[j + 2] == '*'))
				j++;
			printf("<strong><em>");
			render_inline(s + i + 3, j - i - 3);
			printf("</em></strong>");
			i = j + (j + 2 < len ? 3 : 0);
			continue;
		}
		if (i + 1 < len && s[i] == '*' && s[i + 1] == '*')
		{
			int j = i + 2;
			while (j + 1 < len && !(s[j] == '*' && s[j + 1] == '*'))
				j++;
			printf("<strong>");
			render_inline(s + i + 2, j - i - 2);
			printf("</strong>");
			i = j + (j + 1 < len ? 2 : 0);
			continue;
		}
		if ((s[i] == '*' || (s[i] == '_' && (i == 0 || s[i - 1] == ' '))) && i + 1 < len)
		{
			char delim = s[i];
			int	 j	   = i + 1;
			while (j < len && s[j] != delim)
				j++;
			printf("<em>");
			render_inline(s + i + 1, j - i - 1);
			printf("</em>");
			i = j + (j < len ? 1 : 0);
			continue;
		}
		if (s[i] == '[')
		{
			int tj = i + 1;
			while (tj < len && s[tj] != ']')
				tj++;
			if (tj < len && tj + 1 < len && s[tj + 1] == '(')
			{
				int uj = tj + 2;
				while (uj < len && s[uj] != ')')
					uj++;
				printf("<a href='");
				he(s + tj + 2, uj - tj - 2);
				printf("'>");
				render_inline(s + i + 1, tj - i - 1);
				printf("</a>");
				i = uj + (uj < len ? 1 : 0);
				continue;
			}
		}
		if (i + 1 < len && s[i] == '~' && s[i + 1] == '~')
		{
			int j = i + 2;
			while (j + 1 < len && !(s[j] == '~' && s[j + 1] == '~'))
				j++;
			printf("<del>");
			render_inline(s + i + 2, j - i - 2);
			printf("</del>");
			i = j + (j + 1 < len ? 2 : 0);
			continue;
		}
		switch (s[i])
		{
			case '&':
				printf("&amp;");
				break;
			case '<':
				printf("&lt;");
				break;
			case '>':
				printf("&gt;");
				break;
			default:
				putchar(s[i]);
		}
		i++;
	}
}

static void render_markdown(const char *src)
{
	char **lines  = (char **) malloc(8192 * sizeof(char *));
	int	   nlines = 0;
	char  *copy	  = (char *) malloc(strlen(src) + 2);
	strcpy(copy, src);
	char *p = copy;
	while (*p && nlines < 8192)
	{
		lines[nlines++] = p;
		char *nl		= strchr(p, '\n');
		if (!nl)
			break;
		*nl = '\0';
		p	= nl + 1;
	}
	int in_ul = 0, in_ol = 0, in_pre = 0, in_bq = 0, in_p = 0;
	for (int i = 0; i < nlines; i++)
	{
		char *line = lines[i];
		int	  len  = (int) strlen(line);
		if (len && line[len - 1] == '\r')
			line[--len] = '\0';
		if (!in_pre && len >= 3 && line[0] == '`' && line[1] == '`' && line[2] == '`')
		{
			if (in_p)
			{
				printf("</p>\n");
				in_p = 0;
			}
			if (in_ul)
			{
				printf("</ul>\n");
				in_ul = 0;
			}
			if (in_ol)
			{
				printf("</ol>\n");
				in_ol = 0;
			}
			if (in_bq)
			{
				printf("</blockquote>\n");
				in_bq = 0;
			}
			char *lang = line + 3;
			while (isspace((unsigned char) *lang))
				lang++;
			if (*lang)
				printf("<pre><code class='language-%s'>", lang);
			else
				printf("<pre><code>");
			in_pre = 1;
			continue;
		}
		if (in_pre)
		{
			if (len >= 3 && line[0] == '`' && line[1] == '`' && line[2] == '`')
			{
				printf("</code></pre>\n");
				in_pre = 0;
			}
			else
			{
				hes(line);
				putchar('\n');
			}
			continue;
		}
		if (len == 0)
		{
			if (in_p)
			{
				printf("</p>\n");
				in_p = 0;
			}
			if (in_ul)
			{
				printf("</ul>\n");
				in_ul = 0;
			}
			if (in_ol)
			{
				printf("</ol>\n");
				in_ol = 0;
			}
			if (in_bq)
			{
				printf("</blockquote>\n");
				in_bq = 0;
			}
			continue;
		}
		if ((line[0] == '-' || line[0] == '*' || line[0] == '_') && len >= 3)
		{
			int same = 1;
			for (int j = 0; j < len; j++)
				if (line[j] != line[0] && line[j] != ' ')
				{
					same = 0;
					break;
				}
			if (same)
			{
				if (in_p)
				{
					printf("</p>\n");
					in_p = 0;
				}
				if (in_ul)
				{
					printf("</ul>\n");
					in_ul = 0;
				}
				printf("<hr>\n");
				continue;
			}
		}
		if (line[0] == '#')
		{
			int lv = 0;
			while (line[lv] == '#' && lv < 6)
				lv++;
			if (line[lv] == ' ')
			{
				if (in_p)
				{
					printf("</p>\n");
					in_p = 0;
				}
				if (in_ul)
				{
					printf("</ul>\n");
					in_ul = 0;
				}
				if (in_ol)
				{
					printf("</ol>\n");
					in_ol = 0;
				}
				if (in_bq)
				{
					printf("</blockquote>\n");
					in_bq = 0;
				}
				printf("<h%d>", lv);
				render_inline(line + lv + 1, len - lv - 1);
				printf("</h%d>\n", lv);
				continue;
			}
		}
		if (line[0] == '>')
		{
			if (in_p)
			{
				printf("</p>\n");
				in_p = 0;
			}
			if (in_ul)
			{
				printf("</ul>\n");
				in_ul = 0;
			}
			if (in_ol)
			{
				printf("</ol>\n");
				in_ol = 0;
			}
			if (!in_bq)
			{
				printf("<blockquote>\n");
				in_bq = 1;
			}
			const char *bq = line + 1;
			if (*bq == ' ')
				bq++;
			render_inline(bq, (int) strlen(bq));
			printf("<br>\n");
			continue;
		}
		else
		{
			if (in_bq)
			{
				printf("</blockquote>\n");
				in_bq = 0;
			}
		}
		if ((line[0] == '-' || line[0] == '*' || line[0] == '+') && len > 1 && line[1] == ' ')
		{
			if (in_p)
			{
				printf("</p>\n");
				in_p = 0;
			}
			if (in_ol)
			{
				printf("</ol>\n");
				in_ol = 0;
			}
			if (!in_ul)
			{
				printf("<ul>\n");
				in_ul = 1;
			}
			printf("<li>");
			render_inline(line + 2, len - 2);
			printf("</li>\n");
			continue;
		}
		if (isdigit((unsigned char) line[0]))
		{
			int j = 0;
			while (j < len && isdigit((unsigned char) line[j]))
				j++;
			if (j < len && (line[j] == '.' || line[j] == ')') && j + 1 < len && line[j + 1] == ' ')
			{
				if (in_p)
				{
					printf("</p>\n");
					in_p = 0;
				}
				if (in_ul)
				{
					printf("</ul>\n");
					in_ul = 0;
				}
				if (!in_ol)
				{
					printf("<ol>\n");
					in_ol = 1;
				}
				printf("<li>");
				render_inline(line + j + 2, len - j - 2);
				printf("</li>\n");
				continue;
			}
		}
		if (line[0] == '|')
		{
			int is_sep = 1;
			for (int j = 0; j < len; j++)
				if (line[j] != '|' && line[j] != '-' && line[j] != ':' && line[j] != ' ')
				{
					is_sep = 0;
					break;
				}
			if (is_sep)
				continue;
			if (in_p)
			{
				printf("</p>\n");
				in_p = 0;
			}
			if (in_ul)
			{
				printf("</ul>\n");
				in_ul = 0;
			}
			printf("<tr>");
			char *cell = strtok(line + 1, "|");
			while (cell)
			{
				while (*cell == ' ')
					cell++;
				int cl = (int) strlen(cell);
				while (cl > 0 && cell[cl - 1] == ' ')
					cl--;
				printf("<td>");
				render_inline(cell, cl);
				printf("</td>");
				cell = strtok(NULL, "|");
			}
			printf("</tr>\n");
			continue;
		}
		if (in_ul)
		{
			printf("</ul>\n");
			in_ul = 0;
		}
		if (in_ol)
		{
			printf("</ol>\n");
			in_ol = 0;
		}
		if (!in_p)
		{
			printf("<p>");
			in_p = 1;
		}
		else
			printf(" ");
		int hbr = (len >= 2 && line[len - 1] == ' ' && line[len - 2] == ' ');
		render_inline(line, hbr ? len - 2 : len);
		if (hbr)
			printf("<br>");
	}
	if (in_p)
		printf("</p>\n");
	if (in_ul)
		printf("</ul>\n");
	if (in_ol)
		printf("</ol>\n");
	if (in_bq)
		printf("</blockquote>\n");
	if (in_pre)
		printf("</code></pre>\n");
	free(copy);
	free(lines);
}

static const char *SAMPLE =
	"# Welcome to the Markdown Renderer\n\n"
	"This CGI script parses **Markdown** entirely in *C* with zero dependencies.\n\n"
	"## Supported Features\n\n"
	"- **Bold**, *italic*, ***bold italic***\n"
	"- `inline code` and fenced code blocks\n"
	"- [Hyperlinks](https://example.com)\n"
	"- ~~Strikethrough~~\n"
	"- Ordered and unordered lists\n"
	"- Blockquotes, tables, horizontal rules\n\n"
	"## Code Example\n\n"
	"```c\n"
	"int main(void) {\n"
	"    printf(\"Hello from CGI!\\n\");\n"
	"    return 0;\n"
	"}\n"
	"```\n\n"
	"> *Any sufficiently advanced CGI is indistinguishable from magic.*\n\n"
	"---\n\n"
	"| Feature     | Status |\n"
	"|-------------|--------|\n"
	"| Headings    | yes    |\n"
	"| Lists       | yes    |\n"
	"| Tables      | yes    |\n"
	"| Code blocks | yes    |\n\n"
	"Paste your own markdown and hit **Render**.\n";

int main(void)
{
	const char *method	 = getenv("REQUEST_METHOD");
	const char *clen_s	 = getenv("CONTENT_LENGTH");
	char	   *md_input = NULL;
	if (method && !strcmp(method, "POST") && clen_s)
	{
		int clen = atoi(clen_s);
		if (clen > 0 && clen < MAX_BODY)
		{
			char *body = (char *) malloc(clen + 1);
			int	  n	   = (int) fread(body, 1, clen, stdin);
			body[n]	   = '\0';
			md_input   = get_param(body, "md");
			free(body);
		}
	}
	const char *src = (md_input && *md_input) ? md_input : SAMPLE;
	printf("Content-Type: text/html; charset=utf-8\r\n\r\n");
	printf("<!DOCTYPE html><html lang='en'><head>\n"
		   "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>\n"
		   "<title>md to html</title>\n"
		   "<link href='https://fonts.googleapis.com/css2?family=Lora:ital,wght@0,400;0,600;1,400&family=Fira+Code:wght@400;500&family=Inter:wght@400;500&display=swap' rel='stylesheet'>\n"
		   "<style>\n"
		   "*{box-sizing:border-box;margin:0;padding:0}\n"
		   "body{background:#faf9f6;color:#1a1814;font-family:'Inter',sans-serif;font-size:15px;min-height:100vh}\n"
		   ".layout{display:grid;grid-template-columns:1fr 1fr;min-height:100vh}\n"
		   "@media(max-width:700px){.layout{grid-template-columns:1fr}}\n"
		   ".input-panel{background:#1e1c18;padding:2rem;display:flex;flex-direction:column;gap:1rem}\n"
		   ".panel-label{font-size:.65rem;letter-spacing:.2em;text-transform:uppercase;color:#5a5540;font-family:'Fira Code',monospace}\n"
		   "textarea{flex:1;background:#141210;border:1px solid #2a2820;border-radius:4px;color:#c8c4b4;"
		   "font-family:'Fira Code',monospace;font-size:.82rem;line-height:1.7;padding:1rem;"
		   "resize:none;min-height:500px;outline:none;transition:border-color .2s}\n"
		   "textarea:focus{border-color:#8b7355}\n"
		   "button{background:linear-gradient(135deg,#8b7355,#c9a87a);color:#1e1c18;"
		   "font-family:'Inter',sans-serif;font-weight:500;font-size:.78rem;letter-spacing:.1em;"
		   "text-transform:uppercase;border:none;padding:.7rem 1.5rem;border-radius:3px;"
		   "cursor:pointer;transition:opacity .2s;align-self:flex-start}\n"
		   "button:hover{opacity:.85}\n"
		   ".output-panel{background:#faf9f6;padding:2.5rem 3rem;overflow-y:auto}\n"
		   ".output-label{font-size:.65rem;letter-spacing:.2em;text-transform:uppercase;"
		   "color:#b0a890;font-family:'Fira Code',monospace;margin-bottom:2rem}\n"
		   ".md h1,.md h2,.md h3,.md h4,.md h5,.md h6{font-family:'Lora',serif;font-weight:600;color:#1a1814;line-height:1.25;margin:1.8rem 0 .8rem}\n"
		   ".md h1{font-size:2rem;border-bottom:2px solid #e8e4da;padding-bottom:.5rem}\n"
		   ".md h2{font-size:1.45rem;border-bottom:1px solid #e8e4da;padding-bottom:.3rem}\n"
		   ".md h3{font-size:1.15rem}\n"
		   ".md p{line-height:1.8;color:#3a3530;margin:.8rem 0}\n"
		   ".md a{color:#8b5e3c;text-decoration:underline;text-underline-offset:3px}\n"
		   ".md strong{font-weight:600;color:#1a1814}\n"
		   ".md em{font-style:italic;font-family:'Lora',serif}\n"
		   ".md del{opacity:.5}\n"
		   ".md code{font-family:'Fira Code',monospace;font-size:.82em;background:#f0ede6;padding:.15em .4em;border-radius:3px;color:#6b4423}\n"
		   ".md pre{background:#1e1c18;border-radius:6px;padding:1.2rem 1.4rem;overflow-x:auto;margin:1.2rem 0}\n"
		   ".md pre code{background:none;color:#c8c4b4;font-size:.83rem;padding:0;border-radius:0}\n"
		   ".md ul,.md ol{padding-left:1.6rem;margin:.8rem 0}\n"
		   ".md li{line-height:1.8;color:#3a3530;margin:.2rem 0}\n"
		   ".md blockquote{border-left:3px solid #c9a87a;padding:.5rem 1.2rem;margin:1rem 0;background:#f5f2eb;border-radius:0 4px 4px 0;color:#6b6050}\n"
		   ".md hr{border:none;border-top:1px solid #e8e4da;margin:1.5rem 0}\n"
		   ".md table{border-collapse:collapse;width:100%%;margin:1rem 0}\n"
		   ".md td{border:1px solid #e0dbd0;padding:.5rem .8rem;font-size:.88rem;color:#3a3530}\n"
		   ".md tr:nth-child(even){background:#f5f2eb}\n"
		   "</style></head><body>\n"
		   "<div class='layout'>\n"
		   "<div class='input-panel'>\n"
		   "  <p class='panel-label'>markdown source</p>\n"
		   "  <form method='POST' action='' style='display:contents'>\n"
		   "    <textarea name='md' spellcheck='false'>");
	hes(src);
	printf("</textarea>\n"
		   "    <button type='submit'>Render &rarr;</button>\n"
		   "  </form>\n"
		   "</div>\n"
		   "<div class='output-panel'>\n"
		   "  <p class='output-label'>rendered output</p>\n"
		   "  <div class='md'>\n");
	render_markdown(src);
	printf("  </div>\n</div>\n</div>\n</body></html>\n");
	if (md_input)
		free(md_input);
	return 0;
}