
#include <stdint.h>

struct i2c_device
{
  uint8_t device;
};

struct i2c_data
{
  const void *tx_buf;
  size_t tx_len;
  void *rx_buf;
  size_t rx_len;
};



int i2c_init(void);
int i2c_transfer(const struct i2c_device *dev, struct i2c_data *data);
static int _transmit();
static int _receive();
static int _check_ack();


int main(int argc, char **argv)
{
  return 0;
}

int i2c_init(void)
{
  
}