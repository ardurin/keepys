#!/bin/sh
check_create() {
	echo "$2" | ./keepys create $1
	[ $(echo "$2" | ./keepys get $1 | wc -c) = 16 ] || echo "Test #$1 failed"
}
check_get() {
	[ $(echo $2 | ./keepys get $1) = $3 ] || echo "Test #$1 failed"
}
directory=$(mktemp -d)
mkdir "${directory}"/keepys
export XDG_DATA_HOME="${directory}"
printf 'IXcQkW8EhuGAkI4ZjTS6JY9aQsCaqSdqu++hpunCDUFIlcNk6v7MBYjCHeE=\n' > "${directory}"/keepys/1
printf 'S8yDwHJ1A5l5k1D8ecKZ2GNAagn1IfvtXGxyDtzWUZVMB3FPNwSLR0WqFww=\n' > "${directory}"/keepys/2
printf 'IxBm0w72mxrV6l4Fminz6+nPPAFthxXk7a35NTQdSEyNAXu6pB8Q56ezsiE=\n' > "${directory}"/keepys/3
printf 'CjHeSTo3lJkl4fAbRwKA7X7VW6eQzm8SVklCfhKIqTxN6V+kIm+4w8WTOo4=\n' > "${directory}"/keepys/4
printf 'BefeQG/k8CxTMNo7Wyka3odfukLEYgU0kFWIQtGu2N10KfLlpb7B78NuqR4=\n' > "${directory}"/keepys/5
check_get 1 '' 'G!ZDGVU8n6sMbArO'
check_get 2 'dog horse' 'gGpC\@(cxT34ernW'
check_get 3 0123456789 'TLzZgF^4D<$}gsax'
check_get 4 'vH?&4"S0?/L@F8Yk' "Ak#1@'t=)d*2W[1i"
check_get 5 xxxxxxxxxxxxxxxxxxxx 'z&<iR.Fl}*(&y~Pn'
check_create 6 ''
check_create 7 'dog horse'
check_create 8 0123456789
check_create 9 'vH?&4"S0?/L@F8Yk'
check_create 10 xxxxxxxxxxxxxxxxxxxx
