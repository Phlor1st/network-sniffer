# Network Activity Indicator (pcap + pcap-filter)

This implementation matches the task requirements:
- traffic capture via `libpcap`;
- arbitrary filter expression in `pcap-filter` syntax;
- graphical display of network activity over time.

## 1) Build main project (with indicator mode)

Linux/macOS:

```bash
gcc -Iincludes main.c src/map.c src/selection_sort.c src/packetProcrssing.c src/interface.c src/rawSocket.c src/parseArguments.c src/analysisPacket.c src/indicator.c -o my_program -lpcap
```

Windows (Npcap + MinGW example):

```powershell
gcc -Iincludes main.c src/map.c src/selection_sort.c src/packetProcrssing.c src/interface.c src/rawSocket.c src/parseArguments.c src/analysisPacket.c src/indicator.c -o my_program.exe -lwpcap
```

> Note: on Windows install Npcap (with SDK/developer libraries) first.

## 2) Run indicator mode

List interfaces:

```bash
./my_program -l
```

Capture 30 seconds, 1-second buckets, with arbitrary pcap filter:

```bash
./my_program -m INDICATOR -i eth0 -f "tcp port 443 or udp" -t 30 -b 1 -o activity.csv
```

Capture until Ctrl+C:

```bash
./my_program -m INDICATOR -i eth0 -f "host 8.8.8.8"
```

Output format:
- CSV file with `bucket_start_sec,bytes`;
- each row = captured bytes during one time interval.

## 3) Draw graph

Install plotting dependency:

```bash
python -m pip install matplotlib
```

Draw and save graph:

```bash
python plot_activity.py -i activity.csv -o activity.png
```

Draw and open interactive window:

```bash
python plot_activity.py -i activity.csv -o activity.png --show
```

## Useful pcap-filter examples

- `tcp`
- `udp and port 53`
- `host 192.168.1.10`
- `src host 10.0.0.5 and not port 22`
- `(tcp or udp) and dst port 443`
