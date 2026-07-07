.PHONY: all time-bmin time-std run-bmin run-std clean

all: time-bmin time-std

time-bmin:
	bash time_compile.sh bmin

time-std:
	bash time_compile.sh std

run-bmin:
	$(MAKE) -C ../src native
	$(MAKE) -C bmin all
	./bmin/bench

run-std:
	$(MAKE) -C std all
	./std/bench

clean:
	$(MAKE) -C bmin clean
	$(MAKE) -C std clean
