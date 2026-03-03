<?php
function h(string $s): string { return htmlspecialchars($s, ENT_QUOTES, 'UTF-8'); }

$qs   = getenv('QUERY_STRING') ?? '';
parse_str($qs, $get);
$city = trim($get['city'] ?? 'Paris');
if ($city === '') $city = 'Paris';
$city = preg_replace('/[^a-zA-Z0-9 ,\-]/', '', $city);

function curl_get(string $url): string|false {
    $ch = curl_init();
    curl_setopt_array($ch, [
        CURLOPT_URL            => $url,
        CURLOPT_RETURNTRANSFER => true,
        CURLOPT_TIMEOUT        => 6,
        CURLOPT_FOLLOWLOCATION => true,
        CURLOPT_USERAGENT      => 'weather-cgi/1.0',
    ]);
    $res = curl_exec($ch);
    $err = curl_error($ch);
    curl_close($ch);
    return $err ? false : $res;
}

function wmo_desc(int $code): string {
    $map = [
        0=>'Clear sky',1=>'Mainly clear',2=>'Partly cloudy',3=>'Overcast',
        45=>'Fog',48=>'Icy fog',
        51=>'Light drizzle',53=>'Drizzle',55=>'Heavy drizzle',
        61=>'Light rain',63=>'Rain',65=>'Heavy rain',
        71=>'Light snow',73=>'Snow',75=>'Heavy snow',77=>'Snow grains',
        80=>'Light showers',81=>'Showers',82=>'Heavy showers',
        85=>'Snow showers',86=>'Heavy snow showers',
        95=>'Thunderstorm',96=>'Thunderstorm w/ hail',99=>'Thunderstorm w/ heavy hail',
    ];
    return $map[$code] ?? 'Unknown';
}

function weather_emoji(string $d): string {
    $d = strtolower($d);
    if (str_contains($d,'thunder')) return '⛈';
    if (str_contains($d,'snow'))    return '❄️';
    if (str_contains($d,'rain') || str_contains($d,'drizzle') || str_contains($d,'shower')) return '🌧';
    if (str_contains($d,'overcast')) return '☁️';
    if (str_contains($d,'partly'))   return '⛅';
    if (str_contains($d,'fog'))      return '🌫';
    if (str_contains($d,'clear') || str_contains($d,'sunny')) return '☀️';
    if (str_contains($d,'mainly clear')) return '🌤';
    return '🌡';
}

function deg_to_compass(int $deg): string {
    $dirs = ['N','NE','E','SE','S','SW','W','NW'];
    return $dirs[round($deg / 45) % 8];
}

$err = null; $w = null;
$lat = null; $lon = null;
$resolved_name = $city; $country = '';

if (!function_exists('curl_init')) {
    $err = "cURL not available on this server.";
} else {
    $geo_url  = 'https://geocoding-api.open-meteo.com/v1/search?name=' . urlencode($city) . '&count=1&language=en&format=json';
    $geo_json = curl_get($geo_url);
    if ($geo_json === false) {
        $err = "Geocoding request timed out.";
    } else {
        $geo = json_decode($geo_json, true);
        if (empty($geo['results'])) {
            $err = "City not found: " . h($city);
        } else {
            $lat           = $geo['results'][0]['latitude'];
            $lon           = $geo['results'][0]['longitude'];
            $resolved_name = $geo['results'][0]['name'] ?? $city;
            $country       = $geo['results'][0]['country'] ?? '';

            $wx_url = "https://api.open-meteo.com/v1/forecast?latitude=$lat&longitude=$lon"
                    . "&current=temperature_2m,apparent_temperature,relative_humidity_2m,"
                    . "wind_speed_10m,wind_direction_10m,weathercode,visibility,uv_index"
                    . "&daily=weathercode,temperature_2m_max,temperature_2m_min,precipitation_probability_max"
                    . "&wind_speed_unit=kmh&timezone=auto&forecast_days=3";
            $wx_json = curl_get($wx_url);
            if ($wx_json === false) {
                $err = "Weather request timed out.";
            } else {
                $w = json_decode($wx_json, true);
                if (!$w || !isset($w['current'])) { $err = "Could not parse weather data."; $w = null; }
            }
        }
    }
}

$cur      = $w['current'] ?? [];
$daily    = $w['daily']   ?? [];
$temp_c   = isset($cur['temperature_2m'])       ? round($cur['temperature_2m'])        : '?';
$temp_f   = isset($cur['temperature_2m'])       ? round($cur['temperature_2m']*9/5+32) : '?';
$feels    = isset($cur['apparent_temperature']) ? round($cur['apparent_temperature'])  : '?';
$humidity = $cur['relative_humidity_2m']        ?? '?';
$wind     = isset($cur['wind_speed_10m'])       ? round($cur['wind_speed_10m'])        : '?';
$wdir     = isset($cur['wind_direction_10m'])   ? deg_to_compass((int)$cur['wind_direction_10m']) : '?';
$vis      = isset($cur['visibility'])           ? round($cur['visibility']/1000, 1)   : '?';
$uv       = isset($cur['uv_index'])             ? round($cur['uv_index'])              : '?';
$wcode    = $cur['weathercode']                 ?? 0;
$desc     = wmo_desc((int)$wcode);
$emoji    = weather_emoji($desc);

header('Content-Type: text/html; charset=utf-8');
?>
<!DOCTYPE html><html lang="en"><head>
<meta charset="UTF-8"><meta name="viewport" content="width=device-width,initial-scale=1">
<title>weather / <?=h($city)?></title>
<link href="https://fonts.googleapis.com/css2?family=Syne:wght@400;600;800&family=DM+Sans:wght@300;400;500&display=swap" rel="stylesheet">
<style>
:root{--sky:#0d1b2a;--card:rgba(255,255,255,0.05);--border:rgba(255,255,255,0.1);--text:#e8f0fe;--muted:rgba(232,240,254,0.45);--accent:#60a5fa;--warm:#fb923c}
*{box-sizing:border-box;margin:0;padding:0}
body{background:var(--sky);color:var(--text);font-family:'DM Sans',sans-serif;min-height:100vh;padding:2rem 1rem 4rem;
background-image:radial-gradient(ellipse 100% 60% at 50% -20%,rgba(96,165,250,.18) 0%,transparent 70%)}
.container{max-width:820px;margin:0 auto}
.search-bar{display:flex;gap:.6rem;margin-bottom:2.5rem}
.search-bar input{flex:1;background:var(--card);border:1px solid var(--border);border-radius:6px;color:var(--text);
font-family:'DM Sans',sans-serif;font-size:.95rem;padding:.7rem 1.2rem;outline:none;transition:border-color .2s}
.search-bar input:focus{border-color:var(--accent)}
.search-bar button{background:var(--accent);color:#0d1b2a;font-family:'DM Sans',sans-serif;font-weight:500;
font-size:.88rem;border:none;padding:.7rem 1.4rem;border-radius:6px;cursor:pointer;transition:opacity .2s}
.search-bar button:hover{opacity:.85}
.hero{background:var(--card);border:1px solid var(--border);border-radius:16px;padding:2.5rem;margin-bottom:1.2rem;
display:flex;justify-content:space-between;align-items:flex-start;flex-wrap:wrap;gap:1.5rem}
.location{font-family:'Syne',sans-serif;font-size:.75rem;letter-spacing:.2em;text-transform:uppercase;color:var(--muted);margin-bottom:.4rem}
.temp{font-family:'Syne',sans-serif;font-size:clamp(4rem,12vw,6rem);font-weight:800;line-height:1}
.temp sub{font-size:2rem;font-weight:400;color:var(--muted);vertical-align:super}
.desc{margin-top:.5rem;font-size:1.1rem;color:var(--muted)}
.big-emoji{font-size:5rem;line-height:1}
.feels{font-size:.8rem;color:var(--muted);margin-top:.5rem}
.stats{display:grid;grid-template-columns:repeat(auto-fit,minmax(140px,1fr));gap:.8rem;margin-bottom:1.2rem}
.stat-card{background:var(--card);border:1px solid var(--border);border-radius:10px;padding:1rem 1.2rem}
.stat-label{font-size:.65rem;letter-spacing:.15em;text-transform:uppercase;color:var(--muted);margin-bottom:.4rem}
.stat-val{font-family:'Syne',sans-serif;font-size:1.4rem;font-weight:600}
.stat-unit{font-size:.75rem;color:var(--muted);margin-left:2px}
.forecast-grid{display:grid;grid-template-columns:repeat(auto-fit,minmax(150px,1fr));gap:.8rem;margin-bottom:1.2rem}
.forecast-card{background:var(--card);border:1px solid var(--border);border-radius:10px;padding:1rem;text-align:center}
.forecast-day{font-size:.7rem;letter-spacing:.12em;text-transform:uppercase;color:var(--muted);margin-bottom:.5rem}
.forecast-emoji{font-size:2rem;margin-bottom:.4rem}
.forecast-hi{color:var(--warm)}
.forecast-lo{color:var(--muted);margin-left:.4rem}
.forecast-desc{font-size:.7rem;color:var(--muted);margin-top:.3rem}
.forecast-rain{font-size:.7rem;color:var(--accent);margin-top:.2rem}
.error{background:rgba(251,113,133,.1);border:1px solid rgba(251,113,133,.3);border-radius:10px;
padding:1.2rem 1.5rem;color:#fca5a5;font-size:.88rem;margin-bottom:1.5rem}
.footer{text-align:center;font-size:.65rem;color:var(--muted);margin-top:3rem;letter-spacing:.1em}
</style></head><body><div class="container">

<form method="GET" action=""><div class="search-bar">
<input type="text" name="city" value="<?=h($city)?>" placeholder="Enter city…" autofocus>
<button type="submit">Search</button>
</div></form>

<?php if ($err): ?>
<div class="error">⚠ <?=$err?></div>
<?php endif; ?>

<?php if ($w): ?>
<div class="hero">
  <div class="hero-left">
    <p class="location"><?=h($resolved_name)?><?=$country ? ', '.h($country) : ''?></p>
    <div class="temp"><?=h($temp_c)?><sub>°C</sub></div>
    <p class="desc"><?=h($desc)?></p>
  </div>
  <div class="hero-right">
    <div class="big-emoji"><?=$emoji?></div>
    <p class="feels">Feels like <?=h($feels)?>°C · <?=h($temp_f)?>°F</p>
  </div>
</div>

<div class="stats">
  <div class="stat-card"><p class="stat-label">💧 Humidity</p><span class="stat-val"><?=h($humidity)?></span><span class="stat-unit">%</span></div>
  <div class="stat-card"><p class="stat-label">💨 Wind</p><span class="stat-val"><?=h($wind)?></span><span class="stat-unit">km/h <?=h($wdir)?></span></div>
  <div class="stat-card"><p class="stat-label">👁 Visibility</p><span class="stat-val"><?=h($vis)?></span><span class="stat-unit">km</span></div>
  <div class="stat-card"><p class="stat-label">☀️ UV Index</p><span class="stat-val"><?=h($uv)?></span></div>
</div>

<?php if (!empty($daily['time'])): ?>
<div class="forecast-grid">
<?php for ($i = 0; $i < min(3, count($daily['time'])); $i++):
    $hi    = isset($daily['temperature_2m_max'][$i]) ? round($daily['temperature_2m_max'][$i]) : '?';
    $lo    = isset($daily['temperature_2m_min'][$i]) ? round($daily['temperature_2m_min'][$i]) : '?';
    $date  = $daily['time'][$i] ?? '';
    $dcode = $daily['weathercode'][$i] ?? 0;
    $ddesc = wmo_desc((int)$dcode);
    $rain  = $daily['precipitation_probability_max'][$i] ?? 0;
    $de    = weather_emoji($ddesc);
?>
<div class="forecast-card">
  <p class="forecast-day"><?=$date ? date('D d', strtotime($date)) : '—'?></p>
  <div class="forecast-emoji"><?=$de?></div>
  <div><span class="forecast-hi"><?=h($hi)?>°</span><span class="forecast-lo"><?=h($lo)?>°</span></div>
  <p class="forecast-desc"><?=h($ddesc)?></p>
  <p class="forecast-rain">🌧 <?=h($rain)?>%</p>
</div>
<?php endfor; ?>
</div>
<?php endif; ?>
<?php endif; ?>

<p class="footer">powered by open-meteo.com · PHP CGI · no API key needed</p>
</div></body></html>