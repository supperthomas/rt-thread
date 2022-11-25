arm-none-eabi-objcopy --input-target=binary --output-target=ihex --change-address 0x0000 --gap-fill=0xff  rtthread.bin rtthread.hex

::nrfjprog -f NRF53 --coprocessor CP_NETWORK --recover
::nrfjprog -f NRF53 --recover

::nrfjprog -f NRF53 --coprocessor CP_NETWORK --eraseall
::nrfjprog -f NRF53 --coprocessor CP_NETWORK --program merged_CPUNET.hex --verify
::url = https://gitee.com/andrewliNordic/rt_thread_porting_to_nrf5340.git

nrfjprog -f NRF53 --eraseall
nrfjprog -f NRF53 --program rtthread.hex --verify

nrfjprog --debugreset

pause