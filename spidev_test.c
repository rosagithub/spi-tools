/*
 * SPI testing utility (using spidev driver)
 *
 * Copyright (c) 2007  MontaVista Software, Inc.
 * Copyright (c) 2007  Anton Vorontsov <avorontsov@ru.mvista.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * Cross-compile with cross-gcc -I/path/to/cross-kernel/include
 */

#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/types.h>
#include <linux/spi/spidev.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))


static const char *device = "/dev/spidev1.1";
static uint8_t mode;
static uint8_t bits = 8;
static uint32_t speed = 500000;
static uint16_t delay;
static uint8_t size;
static uint8_t *pbuffer = NULL;



static void print_usage(const char *prog)
{
  printf("Usage: %s [-DsbdlHOLC3] [X] \n", prog ? prog : "");
  puts("  -D --device   device to use (default /dev/spidev1.1)\n"
       "  -s --speed    max speed (Hz)\n"
       "  -d --delay    delay (usec)\n"
       "  -b --bpw      bits per word \n"
       "  -l --loop     loopback\n"
       "  -H --cpha     clock phase\n"
       "  -O --cpol     clock polarity\n"
       "  -L --lsb      least significant bit first\n"
       "  -C --cs-high  chip select active high\n"
       "  -3 --3wire    SI/SO signals shared\n"
       "  -X --xdata    hexadecimal data\n" 
       "./spidev_test -D /dev/spidev1.1 -s 26000000 -b 8 -X 0xbb 0xcc\n");
  if(prog)
    exit(1);
}

static void pabort(const char *s)
{
  if(pbuffer != NULL)
    free(pbuffer);
  pbuffer = NULL;
  print_usage(NULL);
  perror(s);
  abort();
}


static void transfer(int fd, uint8_t * tx)
{
  int ret;

  uint8_t *rx;
  rx = malloc(size);
  struct spi_ioc_transfer tr = {
    .tx_buf = (unsigned long)tx,
    .rx_buf = (unsigned long)rx,
    .len = ARRAY_SIZE(tx),
    .delay_usecs = delay,
    .speed_hz = speed,
    .bits_per_word = bits,
  };

  ret = ioctl(fd, SPI_IOC_MESSAGE(1), &tr);
  if(ret < 1)
    pabort("can't send spi message");

  printf("TX: ");
  for(ret = 0; ret < size; ret++)
  {
    if(!(ret % 6) && ret)
      puts("");
    printf("%.2x ", tx[ret]);
  }

  printf("\t RX: ");
  for(ret = 0; ret < size; ret++)
  {
    if(!(ret % 6) && ret)
      puts("");
    printf("%.2x ", rx[ret]);
  }
  puts("");
}

static void parse_opts(int argc, char *argv[])
{
  int i, index;
  char *p;

  while(1)
  {
    static const struct option lopts[] = {
      {"device", 1, 0, 'D'},
      {"speed", 1, 0, 's'},
      {"delay", 1, 0, 'd'},
      {"bpw", 1, 0, 'b'},
      {"loop", 0, 0, 'l'},
      {"cpha", 0, 0, 'H'},
      {"cpol", 0, 0, 'O'},
      {"lsb", 0, 0, 'L'},
      {"cs-high", 0, 0, 'C'},
      {"3wire", 0, 0, '3'},
      {"no-cs", 0, 0, 'N'},
      {"ready", 0, 0, 'R'},
      {"Xdata", 1, 0, 'X'},
      {NULL, 0, 0, 0},
    };
    int c;

    c = getopt_long(argc, argv, "D:s:d:b:lHOLC3NRX", lopts, NULL);

    if(c == -1)
      break;

    switch (c)
    {
      case 'D':
        device = optarg;
        break;
      case 's':
        speed = atoi(optarg);
        break;
      case 'd':
        delay = atoi(optarg);
        break;
      case 'b':
        bits = atoi(optarg);
        break;
      case 'l':
        mode |= SPI_LOOP;
        break;
      case 'H':
        mode |= SPI_CPHA;
        break;
      case 'O':
        mode |= SPI_CPOL;
        break;
      case 'L':
        mode |= SPI_LSB_FIRST;
        break;
      case 'C':
        mode |= SPI_CS_HIGH;
        break;
      case '3':
        mode |= SPI_3WIRE;
        break;
#if 0
      case 'N':
        mode |= SPI_NO_CS;
        break;
      case 'R':
        mode |= SPI_READY;
        break;
#endif
      case 'X':
        i = 0;
        size = argc - optind;
        pbuffer = malloc(size);
        for(index = optind; index < argc; index++, i++)
        {
          pbuffer[i] = strtol(argv[index], &p, 0);
        }
        break;
      default:
        print_usage(argv[0]);
        break;
    }
  }
}

int main(int argc, char *argv[])
{
  int ret = 0;
  int fd;

  pbuffer = NULL;

  parse_opts(argc, argv);

  fd = open(device, O_RDWR);
  if(fd < 0)
    pabort("can't open device");

  /*
   * spi mode
   */
  ret = ioctl(fd, SPI_IOC_WR_MODE, &mode);
  if(ret == -1)
    pabort("can't set spi mode");

  ret = ioctl(fd, SPI_IOC_RD_MODE, &mode);
  if(ret == -1)
    pabort("can't get spi mode");

  /*
   * bits per word
   */
  ret = ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &bits);
  if(ret == -1)
    pabort("can't set bits per word");

  ret = ioctl(fd, SPI_IOC_RD_BITS_PER_WORD, &bits);
  if(ret == -1)
    pabort("can't get bits per word");

  /*
   * max speed hz
   */
  ret = ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &speed);
  if(ret == -1)
    pabort("can't set max speed hz");

  ret = ioctl(fd, SPI_IOC_RD_MAX_SPEED_HZ, &speed);
  if(ret == -1)
    pabort("can't get max speed hz");

  printf("spi mode: %d\n", mode);
  printf("bits per word: %d\n", bits);
  printf("max speed: %d Hz (%d KHz)\n", speed, speed / 1000);

  transfer(fd, pbuffer);

  if(pbuffer != NULL)
    free(pbuffer);
  pbuffer = NULL;

  close(fd);

  return ret;
}
