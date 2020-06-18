# Tìm hiểu về Linux kernel



### Thông tin đồ án

- Đồ án thứ 2 cho môn Hệ Điều Hành (CSC10007-18CLC3), FIT @ HCMUS
- Phân công:
  - Toàn: `randocha` (character device để tạo số ngẫu nhiên)
  - Thiện: `syshook` (module hook vào các system call `open()` và `write()`)
  - Phụng: viết báo cáo, sửa lỗi và hoàn thiện.
- Tiến độ: đã hoàn thành.
- Môi trường lập trình: `Ubuntu 14.04.5 LTS` (phiên bản nhân Linux: `4.4.0-31-generic`)



### `randocha`

- Hướng dẫn make và chạy

  ```
  // Build module
  $ make
  
  // Build user tool
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

- Kiến trúc

  - Character device
  - "User-space" tool

- Cơ cấu (flow)

  - `module_init`
    - Step 1
  - `module_exit`: Overview



### `syshook`

- Hướng dẫn make và chạy

  ```
  // Build module
  $ make
  
  // Nạp module
  # insmod syshook.ko
  
  // Gỡ module
  # rmmod syshook
  
  // Dọn dẹp
  $ make clean
  ```

- Kiến trúc

  - 1 module

- Cơ cấu (flow)

  - `module_init`
    - Step 1
  - `module_exit`: Overview