import celery
from matchlib.matcher import match_all_combinations, match_to_others

logger = celery.utils.log.get_task_logger(__name__)


@celery.shared_task
def match_all_combinations(*args):
    logger.info("Got match all combinations task")
    matches = list(match_all_combinations(*args))
    logger.info("All matched")
    return matches


@celery.shared_task
def match_to_others(*args):
    logger.info("Got match to others task")
    matches = list(match_to_others(*args))
    logger.info("All matched")
    return matches
