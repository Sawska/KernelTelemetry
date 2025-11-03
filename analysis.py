import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import json
import subprocess
import io


LOG_FILE_PATH = "/var/log/kernel_telemetry/app.log"

# Налаштування стилю для графіків
sns.set_theme(style="whitegrid")
plt.rcParams['figure.figsize'] = (14, 8)
plt.rcParams['font.size'] = 10


def load_data_from_log(file_path):
    """
    Зчитує JSONL файл за допомогою команди `sudo cat`, оскільки 
    логі зазвичай вимагають прав root, а потім завантажує його в Pandas.
    """
    print(f"Reading log file (requires sudo permissions): {file_path}")
    try:

        result = subprocess.run(['sudo', 'cat', file_path], 
                                capture_output=True, text=True, check=True)
        
        if not result.stdout.strip():
            print("Log file is empty or contains no data.")
            return pd.DataFrame()
            
        data = io.StringIO(result.stdout)
        df = pd.read_json(data, lines=True)
        return df
    
    except subprocess.CalledProcessError as e:
        print(f"Error executing sudo cat: {e.stderr}")
        print("Please ensure the log file exists and you have sudo permissions.")
        return pd.DataFrame()
    except Exception as e:
        print(f"An error occurred during data loading: {e}")
        return pd.DataFrame()


def prepare_data(df):
    """Витягує вкладені дані, перетворює часові мітки та агрегує."""
    if df.empty:
        return df
    
    if 'timestamp' in df.columns:
        df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')
        df = df.set_index('timestamp')
    else:
        print("WARNING: 'timestamp' column missing from log. Using synthetic 1-second index.")
        df.index = pd.to_datetime(pd.Series(range(len(df))) * 1000, unit='ms')
        df.index.name = 'timestamp'

    df['cpu_usage'] = df['cpu'].apply(lambda x: x.get('usagePercentage', 0))
    df['cpu_iowait'] = df['cpu'].apply(lambda x: x.get('iowaitPercentage', 0))
    
    df['mem_available_mb'] = df['memory'].apply(lambda x: x.get('memAvailable', 0) / 1024)
    df['mem_total_mb'] = df['memory'].apply(lambda x: x.get('memTotal', 0) / 1024)
    df['mem_used_mb'] = df['mem_total_mb'] - df['mem_available_mb']

    def calculate_total_disk_writes(disks_list):
        total_writes = 0
        for disk in disks_list:
            total_writes += disk.get('megabytesWrittenPerSecond', 0)
        return total_writes
        
    df['disk_write_mbps'] = df['disks'].apply(calculate_total_disk_writes)
    
    df_agg = df[['cpu_usage', 'cpu_iowait', 'mem_used_mb', 'disk_write_mbps']].resample('1T').mean()

    return df_agg.dropna()


def plot_data(df):
    """Створює діагностичні графіки."""
    if df.empty:
        print("No aggregated data to plot.")
        return

    fig, axes = plt.subplots(3, 1, sharex=True, figsize=(16, 12))
    fig.suptitle('System Telemetry Diagnostic (1 Minute Average)', fontsize=16)

    df[['cpu_usage', 'cpu_iowait']].plot(ax=axes[0], title='CPU Usage and IOWait (%)', linewidth=2)
    axes[0].set_ylabel('Percentage (%)')
    axes[0].legend(loc='upper right')

    df['mem_used_mb'].plot(ax=axes[1], title='Memory Usage (Used MB)', color='darkorange', linewidth=2)
    axes[1].set_ylabel('MB')
    
    df['disk_write_mbps'].plot(ax=axes[2], title='Total Disk Write Rate (MB/s)', color='darkgreen', linewidth=2)
    axes[2].set_ylabel('MB/s')
    axes[2].set_xlabel('Time')


    plt.tight_layout(rect=[0, 0.03, 1, 0.98])
    plt.show()

if __name__ == "__main__":
    raw_df = load_data_from_log(LOG_FILE_PATH)
    
    if not raw_df.empty:
        agg_df = prepare_data(raw_df)
        plot_data(agg_df)
        print("\n--- Diagnostic Data Sample (1 Minute Average) ---")
        print(agg_df.head())
        print("\nVisualization complete.")
    else:
        print("Exiting analysis. Please run the C++ daemon for some time to collect data.")
