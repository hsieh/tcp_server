PATH=$PATH:/sbin;

sysctl -w net.ipv4.conf.default.rp_filter=1
sysctl -w net.ipv4.conf.all.rp_filter=1
sysctl -w net.ipv4.tcp_syncookies=0
sysctl -w net.ipv6.conf.all.disable_ipv6=1
sysctl -w net.ipv4.tcp_max_syn_backlog=819200
sysctl -w net.ipv4.tcp_synack_retries=1
sysctl -w net.ipv4.tcp_max_tw_buckets=819200

sysctl -w net.ipv4.tcp_tw_reuse=1
#sysctl -w net.ipv4.tcp_tw_recycle=1
sysctl -w net.ipv4.tcp_timestamps=1
