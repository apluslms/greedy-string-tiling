import celery
from matchlib import matcher

logger = celery.utils.log.get_task_logger(__name__)


@celery.shared_task
def match_all_combinations(config, *args):
    logger.info("Got match all combinations task")
    matches = list(matcher.match_all_combinations(config, *args))
    logger.info("All matched")
    return {"meta": matcher.RESULT_KEYS, "results": matches, "config": config}


@celery.shared_task
def match_to_others(config, *args):
    logger.info("Got match to others task")
    matches = list(matcher.match_to_others(config, *args))
    logger.info("All matched")
    return {"meta": matcher.RESULT_KEYS, "results": matches, "config": config}
