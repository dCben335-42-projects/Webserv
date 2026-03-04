#!/bin/bash

# ── config: edit these to match your log paths ────────────────
LOG_FILES=(
    "files/logs/access.log"
    "files/logs/error.log"
)
MAX_LINES=200

# ── helpers ───────────────────────────────────────────────────
he() { sed 's/&/\&amp;/g;s/</\&lt;/g;s/>/\&gt;/g;s/"/\&quot;/g' <<< "$1"; }
url_decode() { local s="${1//+/ }"; printf '%b' "${s//%/\\x}"; }
get_param() { echo "$1" | tr '&' '\n' | grep "^${2}=" | head -1 | cut -d= -f2-; }

# ── parse query string ────────────────────────────────────────
QS="${QUERY_STRING:-}"
SELECTED=$(url_decode "$(get_param "$QS" "log")")
FILTER=$(url_decode "$(get_param "$QS" "filter")")
LINES=$(get_param "$QS" "lines")
LINES="${LINES:-100}"
[[ "$LINES" -lt 1   ]] 2>/dev/null && LINES=100
[[ "$LINES" -gt $MAX_LINES ]] 2>/dev/null && LINES=$MAX_LINES

# ── validate selected log (whitelist only) ────────────────────
VALID_LOG=""
for f in "${LOG_FILES[@]}"; do
    [[ "$SELECTED" == "$f" && -f "$f" && -r "$f" ]] && VALID_LOG="$f" && break
done

printf "Content-Type: text/html; charset=utf-8\r\n\r\n"

# ── HTML head + CSS ───────────────────────────────────────────
printf '%s' '<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>logviewer</title>
<link href="https://fonts.googleapis.com/css2?family=JetBrains+Mono:wght@300;400;500&display=swap" rel="stylesheet">
<style>
*{box-sizing:border-box;margin:0;padding:0}
:root{--bg:#0c0c0e;--surface:#111114;--border:#222228;--text:#d4d4d8;--muted:#52525b;--accent:#22d3ee;--green:#4ade80;--red:#f87171;--yellow:#fbbf24}
body{background:var(--bg);color:var(--text);font-family:"JetBrains Mono",monospace;font-size:13px;min-height:100vh;
background-image:radial-gradient(ellipse 80% 40% at 50% -10%,rgba(34,211,238,.06) 0%,transparent 60%)}
.layout{display:grid;grid-template-columns:260px 1fr;min-height:100vh}
@media(max-width:700px){.layout{grid-template-columns:1fr}}
.sidebar{background:var(--surface);border-right:1px solid var(--border);padding:1.5rem 1rem;display:flex;flex-direction:column;gap:1.5rem}
.logo{font-size:.95rem;font-weight:500;color:var(--accent)}
.logo span{color:var(--muted)}
.stitle{font-size:.6rem;letter-spacing:.2em;text-transform:uppercase;color:var(--muted);margin-bottom:.5rem}
.log-list{display:flex;flex-direction:column;gap:.2rem}
.log-item{display:block;padding:.45rem .7rem;border-radius:3px;text-decoration:none;
  color:var(--muted);font-size:.78rem;border:1px solid transparent;
  white-space:nowrap;overflow:hidden;text-overflow:ellipsis;transition:all .15s}
.log-item:hover{color:var(--text);background:rgba(255,255,255,.04)}
.log-item.active{color:var(--accent);background:rgba(34,211,238,.08);border-color:rgba(34,211,238,.2)}
.log-item.missing{color:#3f3f46;cursor:default}
.badge{float:right;font-size:.6rem;padding:.1rem .3rem;border-radius:2px;margin-left:.3rem}
.badge.ok{background:rgba(74,222,128,.15);color:var(--green)}
.badge.no{background:rgba(63,63,70,.3);color:#52525b}
.ff{display:flex;flex-direction:column;gap:.5rem}
.ff input,.ff select{background:var(--bg);border:1px solid var(--border);border-radius:3px;
  color:var(--text);font-family:"JetBrains Mono",monospace;font-size:.78rem;
  padding:.4rem .6rem;outline:none;width:100%;transition:border-color .2s}
.ff input:focus,.ff select:focus{border-color:var(--accent)}
.ff button{background:var(--accent);color:#0c0c0e;font-family:"JetBrains Mono",monospace;
  font-weight:500;font-size:.75rem;border:none;padding:.4rem;border-radius:3px;cursor:pointer}
.ff button:hover{opacity:.85}
.main{display:flex;flex-direction:column;min-width:0}
.topbar{padding:1rem 1.5rem;border-bottom:1px solid var(--border);
  display:flex;justify-content:space-between;align-items:center;
  flex-wrap:wrap;gap:.5rem;background:var(--surface)}
.topbar-title{font-size:.82rem;color:var(--text)}
.topbar-title span{color:var(--muted);margin-left:.5rem;font-size:.72rem}
.stats{display:flex;gap:1rem;font-size:.68rem;color:var(--muted)}
.dot{width:6px;height:6px;border-radius:50%;background:var(--green);display:inline-block;margin-right:.3rem}
.log-wrap{overflow:auto;padding:1rem 1.5rem}
.log-empty{padding:4rem;text-align:center;color:var(--muted);font-size:.82rem}
pre{font-family:"JetBrains Mono",monospace;font-size:.78rem;line-height:1.7;white-space:pre-wrap;word-break:break-all}
.l-err{color:var(--red)}
.l-warn{color:var(--yellow)}
.l-ok{color:var(--green)}
.l-info{color:var(--accent)}
.l-dim{color:#3f3f46}
.l-norm{color:var(--text)}
.footer{padding:.6rem 1.5rem;border-top:1px solid var(--border);
  font-size:.62rem;color:var(--muted);display:flex;justify-content:space-between}
</style></head><body><div class="layout">
'

# ── sidebar ───────────────────────────────────────────────────
echo '<aside class="sidebar">'
echo '<div class="logo">// <span>log</span>viewer</div>'
echo '<div><p class="stitle">log files</p><div class="log-list">'

for f in "${LOG_FILES[@]}"; do
    fname=$(basename "$f")
    active=""
    [[ "$f" == "$VALID_LOG" ]] && active=" active"
    if [[ -f "$f" && -r "$f" ]]; then
        size=$(du -sh "$f" 2>/dev/null | cut -f1)
        fenc=$(python3 -c "import urllib.parse,sys; print(urllib.parse.quote(sys.argv[1]))" "$f" 2>/dev/null || echo "$f")
        href="?log=${fenc}&lines=${LINES}&filter=$(he "$FILTER")"
        echo "<a class='log-item${active}' href='$(he "$href")'>${fname}<span class='badge ok'>${size}</span></a>"
    else
        echo "<span class='log-item missing'>${fname}<span class='badge no'>n/a</span></span>"
    fi
done

echo '</div></div>'

# filter form
echo '<div><p class="stitle">filters</p>'
echo '<form class="ff" method="GET" action="">'
echo "<input type='hidden' name='log' value='$(he "$VALID_LOG")'>"
echo "<input type='text' name='filter' value='$(he "$FILTER")' placeholder='grep pattern…'>"
echo '<select name="lines">'
for n in 50 100 200; do
    sel=""; [[ "$LINES" == "$n" ]] && sel=" selected"
    echo "<option value='$n'$sel>last $n lines</option>"
done
echo '</select><button type="submit">Apply</button></form></div>'
echo '</aside>'

# ── main panel ────────────────────────────────────────────────
echo '<main class="main">'

if [[ -z "$VALID_LOG" ]]; then
    echo '<div class="topbar"><span class="topbar-title">select a log file</span></div>'
    echo '<div class="log-empty">← choose a log file from the sidebar</div>'
else
    total=$(wc -l < "$VALID_LOG" 2>/dev/null || echo "?")
    modified=$(date -r "$VALID_LOG" "+%Y-%m-%d %H:%M" 2>/dev/null \
               || stat -c "%y" "$VALID_LOG" 2>/dev/null | cut -d. -f1)
    filter_label=""
    [[ -n "$FILTER" ]] && filter_label=" · grep: $(he "$FILTER")"

    echo "<div class='topbar'>"
    echo "  <span class='topbar-title'>$(he "$VALID_LOG")<span>— last $(he "$LINES") lines${filter_label}</span></span>"
    echo "  <div class='stats'><span><span class='dot'></span>${total} total lines</span><span>modified: ${modified}</span></div>"
    echo "</div>"
    echo "<div class='log-wrap'><pre>"

    if [[ -n "$FILTER" ]]; then
        content=$(tail -n "$LINES" "$VALID_LOG" 2>/dev/null | grep -i -- "$(he "$FILTER")" 2>/dev/null)
    else
        content=$(tail -n "$LINES" "$VALID_LOG" 2>/dev/null)
    fi

    if [[ -z "$content" ]]; then
        echo "</pre><div class='log-empty'>no lines matched</div>"
    else
        while IFS= read -r line; do
            esc=$(he "$line")
            lower="${line,,}"
            cls="l-norm"
            grep -qE '(error|err[^o]|fatal|crit|emerg|panic|fail)' <<< "$lower" && cls="l-err"
            grep -qE '(warn)'                                        <<< "$lower" && cls="l-warn"
            grep -qE '(success|started|sent|ready|listening|accepted)'   <<< "$lower" && cls="l-ok"
            grep -qE '(info|debug|notice)'                          <<< "$lower" && cls="l-info"
            echo "<span class='${cls}'>${esc}</span>"
        done <<< "$content"
        echo "</pre>"
    fi
fi

echo '<div class="footer">'
echo "  <span>bash cgi · $(date -u "+%Y-%m-%d %H:%M:%S UTC")</span>"
echo "  <span>running as: $(whoami 2>/dev/null)</span>"
echo '</div>'
echo '</main></div></body></html>'