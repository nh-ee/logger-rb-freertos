## 🧾 Ring Buffer–based Logger Module with Timestamp

<p align="center">
  <img src="Docs/Firmware/images/arch.png" width="700" alt="Logger Architecture">
</p>

### Overview
This demo implements a **low-latency, thread-safe logging system** for FreeRTOS-based applications.  
Logs are stored in a **ring buffer** and transmitted via **UART using DMA**, ensuring minimal impact on real-time baviour of application.

### Key Objectivies
- Asynchronous logging system in RTOS-based application
- Logger module hardware dependencies abstraction with function pointers
- Intergration of FreeRTOS from original source along with SEGGER SystemView
- Flushed buffered logs from FreeRTOS Idle Hook to minimize runtime impact

### Architecture Highlights
- **Ring Buffer** used for temporary log storage
- **FreeRTOS Mutex** protects shared buffer access (write)
- **UART + DMA** handles log transmission
- **Idle Hook** triggers log flushing when CPU is idle
- **Function pointers** decouple logger core from hardware drivers

### Technologies Used
- C (function pointers, modular design)
- Ring Buffer
- FreeRTOS (tasks, mutexes, Idle Hook) with SEGGER SystemView
- UART with DMA
- STM32 HAL

### How to Use

#### 1️. Required Source Files and Includes
| SL | File | Purpose |
|----|------|---------|
| 1. | User/AppSrc/Utils/Src/app_log.c | Core logger implementation |
| 2. | User/AppSrc/Utils/Inc/app_log.h | Logger API declarations |
| 3. | User/AppSrc/Utils/Inc/app_log_config.h | Optional configuration overrides |
| 4. | User/AppSrc/Utils/Src/rbuf.c | Optional configuration overrides |
| 5. | User/AppSrc/Utils/Inc/rbuf.h | Optional configuration overrides |

#### 2. Implement Logger backend interface and configuration
- Hardware initialization along with interrupt handling
- Logger module configuartion

#### 3. Mandatory API calls from Logger Module
- `app_log_transport_done` must be called once the backend finished the transfer of current chunk.
For example,
```c
void backend_xfer_complete_cb( void ) {
	app_log_transport_done();
}
```
- `app_log_process` to be called from IDLE Hook or desired source location to initiate a backend transfer.




