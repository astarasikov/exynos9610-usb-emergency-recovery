dd if=sboot.bin of=part1.bin bs=$((0x2000)) count=1
dd if=sboot.bin of=part2.bin bs=1 skip=$((0x2000)) count=$((0x15000-0x2000))
dd if=sboot.bin of=part3.bin bs=1 skip=$((0x15000)) count=$((0x44000-0x15000))
dd if=sboot.bin of=part4.bin bs=1 skip=$((0x5a000)) count=$((0x180000))
dd if=sboot.bin of=part5.bin bs=1 skip=$((0x5a000+0x180000)) count=$((0x40000))
dd if=sboot.bin of=part6.bin bs=1 skip=$((0x5a000+0x180000+0x40000)) count=$((0x31B000-0x5a000-0x180000-0x40000))
