#!/bin/bash

LOG_DIR="/var/log/kernel_telemetry"
SERVICE_USER="telemetry_user"
SERVICE_GROUP="telemetry_group"

if ! getent group "$SERVICE_GROUP" > /dev/null; then
    echo "Adding system group: $SERVICE_GROUP"
    groupadd -r "$SERVICE_GROUP"
    if [ $? -ne 0 ]; then
        echo "FATAL: Failed to add group $SERVICE_GROUP" >&2
        exit 1
    fi
else
    echo "Group $SERVICE_GROUP already exists."
fi

if ! getent passwd "$SERVICE_USER" > /dev/null; then
    echo "Adding system user: $SERVICE_USER"
    useradd -r -g "$SERVICE_GROUP" -s /usr/sbin/nologin -c "Telemetry Daemon User" "$SERVICE_USER"
    if [ $? -ne 0 ]; then
        echo "FATAL: Failed to add user $SERVICE_USER" >&2
        exit 1
    fi
else
    echo "User $SERVICE_USER already exists."
fi

echo "Creating or ensuring log directory: $LOG_DIR"
mkdir -p "$LOG_DIR"

echo "Setting ownership for $SERVICE_USER:$SERVICE_GROUP on $LOG_DIR"
chown -R "$SERVICE_USER":"$SERVICE_GROUP" "$LOG_DIR"

chmod 700 "$LOG_DIR"

echo "Setup complete."
exit 0
