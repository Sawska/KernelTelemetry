# 🧠 Kernel Telemetry Daemon (KernelTelemetry)

**High-performance C++ daemon for reliable Linux system monitoring and data logging.**

---

## 1. 📘 Project Overview

**KernelTelemetry** is a professional, lightweight solution for continuous collection of low-level Linux system performance metrics.

The project utilizes a **two-component architecture**:

* **C++ Daemon (Logger):**
  Runs as a minimal-overhead system service (`systemd`), collecting data directly from kernel file systems such as `/proc` and `/sys`.

* **JSONL Logging:**
  All metrics are stored in **JSON Lines (JSONL)** format, making them ready for time-series analysis.

* **Python Analytics:**
  Ideal for processing and visualization using **Pandas**, **Matplotlib**, and **Prophet**.

---

## 2. ⚙️ Key Features

* **🧩 Low Overhead (C++):**
  Designed for efficiency and minimal system performance impact.

* **🔐 Secure System Service:**
  Runs under a dedicated non-login user (`telemetry_user`) via `systemd`.

* **📊 Metric Collection:**
  Captures:

  * CPU usage and IOWait
  * Disk IOPS and throughput
  * Memory utilization

* **🤖 ML/AI Ready:**
  JSONL logs are optimized for:

  * **Time-series forecasting**
  * **Anomaly detection**
  * **Trend analysis** in Python

---

## 3. 🧱 Requirements

### Compilation (C++)

* **GCC** or **Clang** (C++17 or newer)
* **CMake** ≥ 3.14
* **Git** (for external dependencies such as `nlohmann/json`)

### Analysis (Python)

* **Python 3.x**
* **Pandas** — data manipulation
* **Matplotlib / Seaborn** — visualization
* *(Optional)* **Prophet** — forecasting and anomaly detection

---

## 4. 🧩 Installation and Automated Startup

The installation process:

* Creates the `telemetry_user` system user
* Sets permissions
* Configures the `systemd` service
* Automatically starts the daemon

### A. Build the Executable

```bash
# Clone the repository
git clone <your_repository_link>
cd KernelTelemetry

# Create build directory and run CMake
mkdir build && cd build
cmake ..

# Compile the program
make
```

### B. Install and Start the Service (Root Privileges Required)

```bash
sudo make install
```

> 💡 This command installs the executable, sets up the systemd service, and launches the daemon automatically.
> If a previous manual installation failed, rerunning `sudo make install` should now complete the setup correctly.

---

## 5. 🧠 Usage and Diagnostics

### A. Check Daemon Status

```bash
sudo systemctl status kernel-telemetry.service
```

You should see:

```
Active: active (running)
```

### B. View Data Logs (JSONL)

Logs are written to:

```
/var/log/kernel_telemetry/app.log
```

To watch new entries in real time:

```bash
sudo tail -f /var/log/kernel_telemetry/app.log
```

### C. Service Management

| Action            | Command                                           |
| ----------------- | ------------------------------------------------- |
| Stop              | `sudo systemctl stop kernel-telemetry.service`    |
| Start             | `sudo systemctl start kernel-telemetry.service`   |
| Disable Autostart | `sudo systemctl disable kernel-telemetry.service` |

---

## 6. 📈 Data Analysis (Python)

Run the provided analytics script:

```bash
sudo python3 analysis.py
```

### Example Analyses

* **System Diagnosis:**
  Correlate `cpu_iowait` and disk metrics to detect I/O bottlenecks.

* **Forecasting (ML):**
  Use Prophet or similar tools to predict:

  * When CPU utilization may exceed **80%**
  * When available memory may drop below a **critical threshold**

---

## 🧾 License

This project is licensed under the **MIT License** (or specify your license here).

---

## 💡 Author

**KernelTelemetry**
Professional Linux performance monitoring made lightweight and extensible.
