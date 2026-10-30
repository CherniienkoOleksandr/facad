#!/usr/bin/env bash
# window_size_change/window_change.sh — печатает размер терминала на каждый ресайз
#
# cols/lines — из ioctl(TIOCGWINSZ) через stty (то же, что читает facad в main.c)
# window/cell — из CSI-запросов к самому терминалу (14t / 16t), в пикселях
#
# Запуск: ./window_change.sh

set -u
TTY=/dev/tty

csi() { # $1: 18 = клетки, 14 = пиксели окна, 16 = пиксели клетки
    local old r
    old=$(stty -g <"$TTY") || return 1
    stty -echo raw <"$TTY"
    printf '\033[%st' "$1" >"$TTY"
    IFS= read -r -t 0.2 -d t r <"$TTY" || true
    stty "$old" <"$TTY"
    printf '%s' "$r"
}

field() { # $1 = код CSI, $2 = префикс ответа ("4" или "6") -> WxH
    local r
    r=$(csi "$1") || {
        echo "?"
        return
    }
    [[ $r =~ \[$2\;([0-9]+)\;([0-9]+) ]] &&
        echo "${BASH_REMATCH[2]}x${BASH_REMATCH[1]}" || echo "?"
}

report() {
    local rows cols
    read -r rows cols < <(stty size <"$TTY") || {
        rows=0
        cols=0
    }
    # tty без реального размера (перенаправление, пустой pty)
    [ "$cols" = 0 ] && cols="?"
    [ "$rows" = 0 ] && rows="?"
    printf '[%s] cols=%s lines=%s  window=%s  cell=%s\n' \
        "$(date +%H:%M:%S)" "$cols" "$rows" "$(field 14 4)" "$(field 16 6)"
}

report            # начальный размер
trap report WINCH # каждое изменение окна
while :; do
    sleep 3600 &
    wait $!
done
