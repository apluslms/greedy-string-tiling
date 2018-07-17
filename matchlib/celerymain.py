"""
Celery app for running matchlib tasks asynchronously.
For example, if you want to run 4 workers in parallel, consuming from the queue 'large_tasks':
  python3 -m celery worker --app matchlib.celerymain --concurrency 4 --queue large_tasks --loglevel INFO
"""
import os
from celery import Celery
# Import tasks so the Celery app can autodetect them from this module
from matchlib.tasks import match_all_combinations, match_to_others

def make_celery():
    user = os.environ["BROKER_USER"]
    secret_path = os.environ["BROKER_SECRET_PATH"]
    with open(secret_path) as f:
        password = f.read().rstrip()
    address = os.environ["BROKER_ADDRESS"]
    broker_address = f"amqp://{user}:{password}@{address}"
    return Celery("matchlib_celerymain", broker=broker_address)

app = make_celery()

if __name__ == "__main__":
    app.worker_main()
