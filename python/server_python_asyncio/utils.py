def time_now():
    from datetime import datetime
    return str(datetime.now().strftime("%Y%m%dT%H%M%S.%f")[:-3])
