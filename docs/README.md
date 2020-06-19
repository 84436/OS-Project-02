# Tìm hiểu về Linux kernel

## Thông tin đồ án

- Đồ án thứ 2 cho môn Hệ Điều Hành (CSC10007-18CLC3), FIT @ HCMUS
- Nhóm gồm 3 thành viên

  - `18127231`: Đoàn Đình Toàn, [@t3bol90](https://github.com/t3bol90)
  - `18127185`: Bùi Vũ Hiếu Phụng, [@alecmatts](https://github.com/alecmatts)
  - `18127221`: Bùi Văn Thiện, [@84436](https://github.com/84436)
- Môi trường
  - Ngôn ngữ lập trình: `C`, biên dịch với `gcc`
  - Môi trường lập trình: `Ubuntu 14.04.5 LTS` (phiên bản nhân Linux: `4.4.0-31-generic`) 



## Tiến độ

*  Phân công:
    * Toàn: `randocha` (character device để tạo số ngẫu nhiên)
    * Thiện: `syshook` (module hook vào các system call `open()` và `write()`)
    * Phụng: viết báo cáo, sửa lỗi.
* Tiến độ: đã hoàn thành 99% (phần `syshook` xem **Thiết kế** để biết khuyết điểm của cách làm hiện tại)



---



## Về `dmesg`

Trong quá trình nạp/gỡ module và thử nghiệm, ta cần xem các thông báo từ kernel và hệ thống (hay gọi đúng hơn là **d**iagnostic **mes**sa**g**e). Để có thể tiện xem (scroll và tìm kiếm) output của `dmesg`, ta dùng công cụ `less`.

Đối với `dmesg` phiên bản cũ (đã được thử trên môi trường lập trình nói trên), chạy:

```bash
# [F]ollow input
dmesg | less +F
```

Đối với `dmesg` phiên bản mới hơn (`utils-linux >= 2.25`), chạy:

```bash
# follo[-w] input (update khi có thay đổi), [-H]uman-readable output, co[-L]orize;
# output [-R]aw control chars, [F]ollow input
dmesg -H -L=always | less -R +F
```



## `randocha`

### Hướng dẫn nhanh

```bash
// Build module và user tool
$ make
$ gcc -o user_space user_space.c

// Nạp module
# insmod randocha.ko

// Chạy user tool để thử
# ./user_space

// Gỡ module
# rmmod randocha

// Dọn dẹp
$ make clean
```



### Thiết kế

Có 2 thành phần trong `randocha`:

- Module dùng để tạo số ngẫu nhiên và cho phép các chương trình đọc số ngẫu nhiên đó qua `/dev/randocha`
- Công cụ dùng để test: mở `/dev/randocha`, đọc và xuất số ra màn hình



### Cơ cấu (flow)

- **File operations**

  Cấu trúc `file_operations` dùng để định nghĩa các hàm driver interface theo chuẩn kernel cho trước chỉ đến địa chỉ hàm ta muốn, gồm:

  - `dev_open`: open device

      Các tiến trình khác nhau gọi module này một cách riêng biệt, (tại sao cần count thế?) 

  - `dev_read`: Khởi tạo số ngẫu nhiên và trả lại cho người dùng (ở đây là `user_space`)

      //TODO: cơ chế sinh số ngẫu nhiên

  - `dev_release`: Sau khi trả về số ngẫu nhiên, thông báo đóng device

- **Khởi tạo module (init)**

  Gồm các bước được thể hiện qua sơ đồ sau: major number $\rightarrow$ device class $\rightarrow$ device

  * Bước 1: Đăng kí major number cho device với `file_operations` được khai báo trước. Ở đây ta để major number được cấp phát động thay vì gán tĩnh
  * Bước 2: Tạo device class với tên được định nghĩa sẵn
  * Bước 3: Tạo device với các thông tin đã khởi tạo ở bước 1 và 2

  Tất cả các bước đều đã xử lí lỗi, nếu không thực hiện được sẽ đưa ra lỗi tương ứng

- **Hủy module (exit)**

    Ngược lại với quá trình khởi tạo module, quá trình hủy module được thể hiện qua sơ đồ: major device $\rightarrow$ device class $\rightarrow$ major number

    - Quy tắc: Unregister trước khi hủy, trừ những ngoại lệ sau:

      - Device: không thể unregister device
      - Major number: giải phóng major number đăng kí với device bằng hàm `unregister_chrdev`

       

### VD (flow mẫu)



---



## `syshook`

### Hướng dẫn nhanh

```bash
// Tìm địa chỉ sys_call_table tương ứng trên hệ thống và sửa lại source
$ grep -w sys_call_table /boot/System.map-$(uname -r) | cut -d ' ' -f 1
$ nano syshook.c

// Build module
$ make

// Nạp module
# insmod syshook.ko

// Gỡ module
# rmmod syshook

// Dọn dẹp
$ make clean
```



### Thiết kế

Phần `syshook`này chỉ bao gồm một standalone module.

Module hiện tại chưa thể tìm địa chỉ của `sys_call_table` (system call table) một cách linh động nên hiện tại sẽ gán cứng. Địa chỉ này cần được sửa lại (xem **Hướng dẫn nhanh**) trước khi module được build và nạp.



### Cơ cấu (flow)

- Khởi tạo module (init)
  
  - Đọc `sys_call_table`
  - Lưu lại hàm xử lý syscall gốc cho `open()` và `writ
  
- Hủy module (exit)

### VD (flow mẫu)



---



## Tài liệu tham khảo

- Tài liệu của cô
- GitHub repo của anh Khang (@khang99)
- GitHub repo của anh Thế Anh 16CNTN (@vltanh)
- [Sourcerer – "Writing a Simple Linux Kernel Module"](https://blog.sourcerer.io/writing-a-simple-linux-kernel-module-d9dc3762c234)
- [The Linux Kernel Programming Guide – Chapter 2, "Hello World"](https://tldp.org/LDP/lkmpg/2.6/html/c119.html)

