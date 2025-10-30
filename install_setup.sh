#!bin/bash

LOG_DIR="/var/log/kernel_telemetry"
SERVICE_USER="telemetry_user"
SERVICE_GROUP="telemetry_group"

id -g "$SERVICE_GROUP" >/dev/null 2>&1 || groupadd -r "$SERVICE_GROUP"
id -u "$SERVICE_USER" >/dev/null 2>&1 || useradd -r -g "$SERVICE_GROUP" -s /usr/sbin/nologin -c "Telemetry Daemon User" "$SERVICE_USER"

echo "Creating log directory: $LOG_DIR"
mkdir -p "$LOG_DIR"

echo "Setting ownership for $SERVICE_USER:$SERVICE_GROUP"
chown -R "$SERVICE_USER":"$SERVICE_GROUP" "$LOG_DIR"

chmod 700 "$LOG_DIR"

