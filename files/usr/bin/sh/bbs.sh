#!/bin/bash
DATA="bbs_posts.tsv"
touch "$DATA"

he() { sed 's/&/\&amp;/g;s/</\&lt;/g;s/>/\&gt;/g;s/"/\&quot;/g' <<< "$1"; }

url_decode() {
    local s="${1//+/ }"
    printf '%b' "${s//%/\\x}"
}

get_post_field() {
    local body="$1" key="$2"
    local raw; raw=$(echo "$body" | tr '&' '\n' | grep "^${key}=" | head -1 | cut -d= -f2-)
    url_decode "$raw"
}

printf "Content-Type: text/html; charset=utf-8\r\n\r\n"

# handle POST
if [ "$REQUEST_METHOD" = "POST" ] && [ -n "$CONTENT_LENGTH" ]; then
    body=$(dd bs=1 count="$CONTENT_LENGTH" 2>/dev/null)
    nick=$(get_post_field "$body" "nick")
    msg=$(get_post_field "$body"  "msg")
    nick="${nick:0:32}"; msg="${msg:0:300}"
    nick="${nick:-Anonymous}"
    if [ -n "$msg" ]; then
        ts=$(date -u "+%Y-%m-%d %H:%M UTC")
        printf '%s\t%s\t%s\n' "$ts" "$nick" "$msg" >> "$DATA"
    fi
fi

count=$(wc -l < "$DATA")

cat << HTML
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>// BBS</title>
<link href="https://fonts.googleapis.com/css2?family=VT323&family=Share+Tech+Mono&display=swap" rel="stylesheet">
<style>
*{box-sizing:border-box;margin:0;padding:0}
body{background:#0a0a00;color:#33ff33;font-family:'Share Tech Mono',monospace;font-size:14px;
min-height:100vh;padding:1.5rem;
background-image:repeating-linear-gradient(0deg,transparent,transparent 2px,rgba(0,255,0,.015) 2px,rgba(0,255,0,.015) 4px)}
.wrap{max-width:700px;margin:0 auto}
.banner{font-family:'VT323',monospace;font-size:2.8rem;color:#00ff00;
text-shadow:0 0 20px rgba(0,255,0,.6);letter-spacing:.05em;margin-bottom:.2rem}
.sub{font-size:.75rem;color:#1a8f1a;margin-bottom:2rem;letter-spacing:.1em}
.card{border:1px solid #1a4d1a;border-radius:2px;padding:1.2rem;margin-bottom:1rem;
background:rgba(0,255,0,.03)}
.ct{font-size:.65rem;letter-spacing:.2em;color:#1a8f1a;margin-bottom:.8rem}
input,textarea{width:100%;background:#000;border:1px solid #1a4d1a;color:#33ff33;
font-family:'Share Tech Mono',monospace;font-size:.88rem;padding:.5rem .7rem;
outline:none;border-radius:2px;transition:border-color .2s}
input:focus,textarea:focus{border-color:#00ff00;box-shadow:0 0 8px rgba(0,255,0,.2)}
textarea{min-height:70px;resize:vertical;margin-top:.5rem}
.field{margin-bottom:.6rem}
label{font-size:.65rem;letter-spacing:.15em;color:#1a8f1a;display:block;margin-bottom:.3rem}
button{background:transparent;border:1px solid #00ff00;color:#00ff00;
font-family:'Share Tech Mono',monospace;font-size:.82rem;letter-spacing:.15em;
padding:.5rem 1.2rem;cursor:pointer;transition:all .2s;margin-top:.6rem}
button:hover{background:#00ff00;color:#000}
.post{border-bottom:1px solid #0d2e0d;padding:.8rem 0}
.post:last-child{border-bottom:none}
.post-meta{font-size:.7rem;color:#1a8f1a;margin-bottom:.3rem}
.post-nick{color:#00ff00;margin-right:.8rem}
.post-msg{color:#a0ffa0;line-height:1.6;white-space:pre-wrap;word-break:break-word}
.count{font-size:.65rem;color:#1a8f1a;letter-spacing:.1em;margin-bottom:.8rem}
.cursor{animation:blink .8s step-start infinite}
@keyframes blink{50%{opacity:0}}
</style></head><body><div class="wrap">
<div class="banner">// BBS <span class="cursor">█</span></div>
<p class="sub">BULLETIN BOARD SYSTEM · BASH CGI · FLAT FILE STORAGE</p>

<div class="card">
<p class="ct">POST A MESSAGE</p>
<form method="POST" action="">
  <div class="field"><label>HANDLE:</label><input type="text" name="nick" maxlength="32" placeholder="Anonymous"></div>
  <div class="field"><label>MESSAGE:</label><textarea name="msg" maxlength="300" placeholder="Type your message..." required></textarea></div>
  <button type="submit">&gt; SUBMIT</button>
</form>
</div>

<div class="card">
<p class="ct">MESSAGES</p>
<p class="count">[ $count POSTS ON RECORD ]</p>
HTML

# print posts newest first
if [ -s "$DATA" ]; then
    tac "$DATA" | while IFS=$'\t' read -r ts nick msg; do
        ts_esc=$(he "$ts")
        nick_esc=$(he "$nick")
        msg_esc=$(he "$msg")
        cat << POSTHTML
<div class="post">
  <div class="post-meta"><span class="post-nick">&gt; $(echo "$nick_esc")</span><span>$(echo "$ts_esc")</span></div>
  <div class="post-msg">$(echo "$msg_esc")</div>
</div>
POSTHTML
    done
else
    echo "<p style='color:#1a8f1a;font-size:.8rem'>NO MESSAGES YET. BE THE FIRST.</p>"
fi

cat << HTML
</div>
<p style="text-align:center;font-size:.62rem;color:#1a4d1a;margin-top:2rem;letter-spacing:.15em">
POWERED BY BASH · CGI/1.1 · $(date -u "+%Y-%m-%d %H:%M UTC")</p>
</div></body></html>
HTML