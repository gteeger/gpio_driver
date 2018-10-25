GPIO Pin 23 set to register an interrupt. An interrupt is registered
everytime pin 23 sees a rising edge.


TO EXAMINE KERNEL MODULE BEHAVIOUR:
tail /var/log/syslog

TO ADD A DEVICE CONNECTED TO THIS MODULE TO EXECUTE OPERATIONS ON:
sudo mknod -m 666 /dev/GPIO_DEVICE c 420 0

TYPICAL COMMANDS TO TRANSFER FILES OVER SSH:
scp gpio_driver.ko debian@192.168.0.10:/home/debian/projects/gpio_driver/gpio_driver/
scp test debian@192.168.0.10:/home/debian/projects/gpio_driver/gpio_driver/