import pandas as pd
import matplotlib.pyplot as plt
import seaborn as sns
import subprocess
import io
from prophet import Prophet
from datetime import timedelta

LOG_FILE_PATH = "/var/log/kernel_telemetry/app.log"
FORECAST_PERIOD_HOURS = 24  
MIN_DATA_HOURS = 24         

def load_data_from_log(file_path):
    
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
        return pd.DataFrame()
    except Exception as e:
        print(f"An error occurred during data loading: {e}")
        return pd.DataFrame()

def prepare_data_for_prophet(df):

    if df.empty or 'timestamp' not in df.columns:
        return pd.DataFrame()
    

    df['timestamp'] = pd.to_datetime(df['timestamp'], unit='s')
    df = df.set_index('timestamp')

    df['cpu_usage'] = df['cpu'].apply(lambda x: x.get('usagePercentage', 0))

    df_hourly = df['cpu_usage'].resample('H').max().reset_index()

    df_prophet = df_hourly.rename(columns={'timestamp': 'ds', 'cpu_usage': 'y'})

    return df_prophet.dropna()

def run_forecasting(df_prophet):

    if len(df_prophet) < MIN_DATA_HOURS:
        print(f"ERROR: Not enough data for reliable forecasting.  Need at least {MIN_DATA_HOURS} data")
        return None, None
    
    model = Prophet(daily_seasonality=True,yearly_seasonality=False)

    model.fit(df_prophet)

    future = model.make_future_dataframe(periods=FORECAST_PERIOD_HOURS, freq='H')

    forecast = model.predict(future)

    return model, forecast

if __name__ == "__main__":
    raw_df = load_data_from_log(LOG_FILE_PATH)
    
    if raw_df.empty:
        print("Analysis stopped: Data frame is empty.")
    else:
        df_prophet = prepare_data_for_prophet(raw_df)
        
        if df_prophet.empty:
            print("Analysis stopped: Data processing resulted in an empty frame.")
        else:
            model, forecast = run_forecasting(df_prophet)
            
            if forecast is not None:
                print(f"\n--- Forecasting {FORECAST_PERIOD_HOURS} hours ahead ---")
                
                fig = model.plot(forecast)
                fig.suptitle(f'CPU Max Usage Forecast (Next {FORECAST_PERIOD_HOURS} Hours)', fontsize=16)
                
                plt.axhline(y=80, color='r', linestyle='--', label='80% Critical Threshold')
                plt.legend()
                plt.show()
                
                critical_forecast = forecast[forecast['yhat_upper'] >= 80]
                
                if not critical_forecast.empty:
                    first_critical_time = critical_forecast.iloc[0]['ds']
                    print(f"\nCRITICAL WARNING: CPU maximum load is predicted to hit 80% or higher around: {first_critical_time}")
                else:
                    print("\nPrediction: Maximum CPU usage stays safely below 80% threshold.")

                print("\nVisualization complete.")
