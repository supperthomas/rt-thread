

volatile char sector_alig_byte[8] __attribute__((section(".vdev_rom"))) = "9651v.1";

void _invalid()
{
	*sector_alig_byte;
}