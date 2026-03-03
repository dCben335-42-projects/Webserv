#!/usr/bin/env python3
import os, sys, html, hashlib, json, urllib.parse
from datetime import datetime
DATA_FILE = os.path.join(os.path.dirname(os.path.abspath(__file__)), "urls.json")
BASE_URL  = os.environ.get("SCRIPT_NAME", "/cgi-bin/urlshort.py")

def load():
    try:
        with open(DATA_FILE) as f: return json.load(f)
    except: return {}

def save(db):
    with open(DATA_FILE, "w") as f: json.dump(db, f, indent=2)

def make_code(url):
    return hashlib.sha256(url.encode()).hexdigest()[:6]

def h(s): return html.escape(str(s))

def out(body):
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n\r\n" + body)

CSS = (
    ":root{--bg:#09090b;--s:#111115;--b:#27272a;--t:#fafafa;--m:#71717a;--a:#a78bfa;--g:#4ade80;--r:#f87171}"
    "*{box-sizing:border-box;margin:0;padding:0}"
    "body{background:var(--bg);color:var(--t);font-family:ui-monospace,monospace;min-height:100vh;padding:2rem 1rem 4rem;"
    "background-image:radial-gradient(ellipse 70% 40% at 50% 0%,rgba(167,139,250,.12) 0%,transparent 60%)}"
    ".w{max-width:680px;margin:0 auto}"
    "header{margin-bottom:3rem;padding-bottom:1.5rem;border-bottom:1px solid var(--b)}"
    "h1{font-size:1.6rem;font-weight:700;color:var(--a)}"
    ".tag{font-size:.75rem;color:var(--m);margin-top:.3rem}"
    ".card{background:var(--s);border:1px solid var(--b);border-radius:6px;padding:1.6rem;margin-bottom:1.2rem}"
    ".ct{font-size:.65rem;letter-spacing:.2em;text-transform:uppercase;color:var(--m);margin-bottom:1rem}"
    ".row{display:flex;gap:.6rem}"
    "input[type=url]{flex:1;background:var(--bg);border:1px solid var(--b);border-radius:4px;color:var(--t);"
    "font-family:inherit;font-size:.88rem;padding:.65rem 1rem;outline:none;transition:border-color .2s}"
    "input[type=url]:focus{border-color:var(--a)}"
    "button{cursor:pointer;transition:opacity .2s}"
    "button.submit{background:var(--a);color:var(--bg);font-family:inherit;font-weight:600;font-size:.8rem;"
    "border:none;padding:.65rem 1.2rem;border-radius:4px;cursor:pointer;white-space:nowrap}"
    "button.submit:hover{opacity:.85}"
    ".del{border:0; background: transparent;color:var(--m);text-decoration:none;font-size:.75rem;border:1px solid var(--b);border-radius:3px;padding:.15rem .4rem}"
    ".del:hover{border-color:var(--r);color:var(--r)}"
    ".ok{background:var(--bg);border:1px solid var(--g);border-radius:4px;padding:.8rem 1rem;margin-top:.8rem}"
    ".sl{color:var(--g);font-size:.9rem;word-break:break-all}"
    ".err{background:var(--bg);border:1px solid var(--r);border-radius:4px;padding:.7rem 1rem;margin-top:.8rem;color:var(--r);font-size:.82rem}"
    "table{width:100%;border-collapse:collapse}"
    "th{font-size:.62rem;letter-spacing:.15em;text-transform:uppercase;color:var(--m);"
    "text-align:left;padding:.4rem .6rem;border-bottom:1px solid var(--b)}"
    "td{font-size:.8rem;padding:.55rem .6rem;border-bottom:1px solid #1a1a1e;word-break:break-all;vertical-align:middle}"
    "td.hits{color:var(--a);text-align:right;white-space:nowrap}"
    "td.date{color:var(--m);font-size:.7rem;white-space:nowrap}"
    "a.code{color:var(--a);text-decoration:none}"
    "a.code:hover{text-decoration:underline}"
    ".foot{text-align:center;font-size:.62rem;color:var(--m);margin-top:3rem}"
)

def page(result="", db=None):
    if db is None: db = {}
    rows = "".join(
        "<tr>"
        f"<td><a class='code' href='?r={h(c)}'>{h(c)}</a></td>"
        f"<td>{h(e.get('url',''))}</td>"
        f"<td class='hits'>{e.get('hits',0)}</td>"
        f"<td class='date'>{e.get('created','')}</td>"
        f"<td><button class='del' onclick=\"delLink('{h(c)}')\">del</button></td>"
        "</tr>"
        for c, e in sorted(db.items(), key=lambda x: x[1].get("created",""), reverse=True)
    )
    tbl = (
        f"<table><tr><th>code</th><th>url</th><th>hits</th><th>created</th><th></th></tr>{rows}</table>"
        if rows else "<p style='color:var(--m);font-size:.8rem'>No links yet.</p>"
    )
    return (
        "<!DOCTYPE html><html lang='en'><head>"
        "<meta charset='UTF-8'><meta name='viewport' content='width=device-width,initial-scale=1'>"
        f"<title>urlshort</title><style>{CSS}</style></head><body><div class='w'>"
        "<header><h1>// urlshort</h1><p class='tag'>url shortener - python cgi - json storage</p></header>"
        "<div class='card'><p class='ct'>shorten a url</p>"
        "<form method='POST' action=''><div class='row'>"
        "<input type='url' name='url' placeholder='https://example.com/long/url...' required>"
        f"<button class='submit' type='submit'>Shorten</button></div></form>{result}</div>"
        f"<div class='card'><p class='ct'>saved links - {len(db)}</p>{tbl}</div>"
        "<p class='foot'>python cgi - no frameworks - no databases</p>"
        "<script>"
        "function delLink(code) {"
        "  fetch('?delete=' + code, { method: 'DELETE' })"
        "    .then(() => window.location.href = window.location.pathname);"
        "}"
        "</script>"
        "</div></body></html>"
    )

# ── routing ───────────────────────────────────────────────────
method = os.environ.get("REQUEST_METHOD", "GET")
qs     = os.environ.get("QUERY_STRING", "")
params = dict(urllib.parse.parse_qsl(qs, keep_blank_values=True))
db     = load()

if method == "DELETE":
    db.pop(params["delete"], None)
    save(db)
    sys.stdout.write("Content-Type: text/plain; charset=utf-8\r\n\r\n")
    sys.stdout.write("ok")
    sys.exit(0)

if "r" in params:
    code = params["r"]
    if code in db:
        db[code]["hits"] = db[code].get("hits", 0) + 1
        save(db)
        target = db[code]["url"]
        sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n\r\n")
        sys.stdout.write(
            "<!DOCTYPE html><html><head>"
            f"<meta http-equiv='refresh' content='0;url={h(target)}'>"
            f"</head><body>Redirecting to <a href='{h(target)}'>{h(target)}</a>...</body></html>"
        )
    else:
        out(page(f"<div class='err'>Code <b>{h(code)}</b> not found.</div>", db))
    sys.exit(0)

if method == "POST":
    n    = int(os.environ.get("CONTENT_LENGTH", 0))
    raw  = sys.stdin.read(n) if n > 0 else ""
    post = dict(urllib.parse.parse_qsl(raw, keep_blank_values=True))
    url  = post.get("url", "").strip()
    if not url.startswith(("http://", "https://")):
        out(page("<div class='err'>URL must start with http:// or https://</div>", db))
        sys.exit(0)
    code = make_code(url)
    if code not in db:
        db[code] = {"url": url, "hits": 0, "created": datetime.utcnow().strftime("%Y-%m-%d %H:%M")}
        save(db)
    short = f"{BASE_URL}?r={code}"
    sys.stdout.write("Content-Type: text/html; charset=utf-8\r\n\r\n")
    sys.stdout.write(
        f"<!DOCTYPE html><html><head>"
        f"<meta http-equiv='refresh' content='0;url={BASE_URL}?new={code}'>"
        f"</head><body></body></html>"
    )
    sys.exit(0)

out(page("", db))