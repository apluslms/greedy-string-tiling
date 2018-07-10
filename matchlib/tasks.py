import celery
import matcher

logger = celery.utils.log.get_task_logger(__name__)

@celery.shared_task
def match_all(*args):
    logger.info("Got match all task")
    matches = list(matcher.match_all(*args))
    logger.info("All matched")
    return matches
