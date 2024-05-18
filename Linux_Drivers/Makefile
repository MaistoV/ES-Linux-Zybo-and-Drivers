all: user_space kernel_space

user_space: no_driver uio uio-int

kernel_space: hellok custom_gpio_k

no_driver:
	make -C user_space/$@

uio:
	make -C user_space/$@

uio-int:
	make -C user_space/$@

hellok:
# 	Build kernel code
	make -C kernel_space/$@

custom_gpio_k:
# 	Build user code
	make -C kernel_space/$@
# 	Build kernel code
	make -C kernel_space/$@/kernel_module

BINARIES = 	user_space/no_driver/no_driver \
			user_space/uio/uio \
			user_space/uio-int/uio-int \
			kernel_space/hellok/hellok.ko \
			kernel_space/custom_gpio_k/kernel_device \
			kernel_space/custom_gpio_k/kernel_module/custom_gpio_kernel_driver.ko

EXPORT_DIR = ${PWD}/exported_binaries
export_binaries: user_space kernel_space
	mkdir -p ${EXPORT_DIR}
	cp ${BINARIES} ${EXPORT_DIR}

clean: 
	rm -rf ${EXPORT_DIR}
	make -C user_space/no_driver 					 clean
	make -C user_space/uio 							 clean
	make -C user_space/uio-int 						 clean
	make -C kernel_space/hellok 					 clean
	make -C kernel_space/custom_gpio_k/kernel_module clean

# PHONIES
.PHONY: user_space kernel_space