"""
Celery app for running matchlib tasks asynchronously.
For example, if you want to run 4 workers in parallel, consuming from the queue 'large_tasks':
  celery --app matchlib --concurrency 4 --queue large_tasks --loglevel INFO
"""
import os
from celery import Celery
# Import tasks so the Celery app can autodetect them from this module
from matchlib.tasks import match_all_combinations, match_to_others

def make_celery():
    user = os.environ.get("BROKER_USER", "guest")
    password = os.environ.get("BROKER_PASSWORD", "guest")
    address = os.environ.get("BROKER_ADDRESS", "localhost:5672")
    broker_address = f"amqp://{user}:{password}@{address}"
    return Celery("lcs_celery", broker=broker_address)

app = make_celery()

if __name__ == "__main__":
    app.worker_main()
