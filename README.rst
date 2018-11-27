=============
Random Module
=============

Nguyễn Trần Hậu - MSSV 1612180

Linux kernel module tạo ra số ngẫu nhiên bằng character device,
các tiến trình ở userspace có thể open và read các số ngẫu nhiên

Cài đặt
=======

Yêu cầu
-------

Máy tính chạy ubuntu và có cài đặt ::

    sudo apt install build-essential libelf-dev \
        linux-headers-generic

Cài đặt module
--------------

::

    make
    sudo insmod rand_char_device.ko

Gỡ cài đặt module
-----------------

::

    sudo rmmod rand_char_device

Test module
-----------

::

    make test
    sudo ./test_rand_char_device.o

Trong file rand_char_device.c
=============================

Struct
------

::

    static struct file_operations fops;
    // Sử dụng struct file_operations để định nghĩa
    // các hàm open, release, read cho character device

Function
--------

::

    static int __init rand_char_init(void);
    // Hàm được gọi để khởi tạo module,
    // bao gồm đăng kí class, device
    // và khởi tạo mutex để ngăn nhiều tiến trình
    // truy cập character device cùng một lúc

    static void __exit rand_char_exit(void);
    // Hàm được gọi để kết thúc module,
    // hủy đi class, device và mutex đã khởi tạo

    static int rand_char_open(struct inode *, struct file *);
    // Hàm được gọi khi character device bị
    // tiến trình ở userspace mở lên (open),
    // kiểm tra nếu không có tiến trình nào khác
    // đang truy cập vào character device thì mới cho mở

    static int rand_char_release(struct inode *, struct file *);
    // Hàm được gọi khi character device bị
    // tiến trình ở userspace đóng lại (release),
    // mở khóa để cho những tiến trình khác có thể truy cập

    static ssize_t rand_char_read(struct file *, char *, size_t, loff_t *);
    // Hàm được gọi khi character device bị
    // tiến trình ở userspace đọc (read),
    // sử dụng hàm get_random_bytes() và copy_to_user()
    // để tiến trình ở userspace có thể lấy được số ngẫu nhiên
    // từ character device

Trong file test_rand_char_device.c
==================================

::

    file_device = open("/dev/rand_char", O_RDONLY);
    // mở character device rand_char bằng hàm open(),
    // O_RDONLY là chế độ Read only


    status = read(file_device, &receive_num, sizeof(receive_num));
    // đọc character device rand_char bàng hàm read(),
    // receive_num là số ngẫu nhiên nhận được
