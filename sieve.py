import concurrent.futures
import subprocess
import logging
import logging.config
import datetime
import csv
import sys
import configparser
import os


def compile_program():
    """
    Function creates file
    :return: None
    """
    module_logger = logging.getLogger("sieve.compile_program")
    try:
        module_logger.info("Trying to compile program with CMake and Make")
        subprocess.run(["cmake", "CMakeLists.txt"], check=True, stdout=subprocess.DEVNULL)
        subprocess.run("make", check=True, stdout=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        module_logger.warning("CMake or Make failed, trying with regular compilation")
        try:
            subprocess.run(["gcc", "list.c", "main.c", "-lm", "-lgmp", "-o", "quadratic-sieve"], check=True)
        except subprocess.CalledProcessError:
            module_logger.critical("GCC failed, exiting")
            exit()


def sieve(number):
    """
    Function starts sieve with chosen number
    :param number: number to sieve
    :return: dictionary of results and corresponding fieldname values
    """
    module_logger = logging.getLogger("sieve.sieve")
    module_logger.info("Starting sieve")
    before = datetime.datetime.now()
    sieve_result = None
    try:
        sieve_result = subprocess.run(["./quadratic-sieve",
                                       str(number),
                                       "--interval=" + get_setting(path, 'sieve_interval', 'sieve_interval'),
                                       "--over_smooth=" + get_setting(path, 'over_smooth', 'over_smooth')],
                                      capture_output=True, check=True)
    except subprocess.CalledProcessError:
        module_logger.critical("There was error during sieve, exiting")
        exit()
    after = datetime.datetime.now()
    delta = after - before
    module_logger.info("Finished sieving")
    data = [str(number), sieve_result.stdout.decode("UTF-8").split('\n')[-2].split(' ')[-1], str(delta)]
    dic = dict(zip(fieldnames, data))
    print(dic)
    return dic


def csv_dict_writer(path, fieldnames, data):
    """
    Writes CSV using DictWriter
    :param path: path to file
    :param fieldnames: field names
    :param data: data to be written
    :return: None
    """
    module_logger = logging.getLogger("sieve.csv_dict_writer")
    module_logger.info("Saving CSV file")
    with open(path, "w", newline='') as out_file:
        writer = csv.DictWriter(out_file, delimiter=',', fieldnames=fieldnames)
        writer.writeheader()
        for row in data:
            writer.writerow(row)


def create_config(path):
    """
    Create a config file
    :param path: path to file
    :return: None
    """
    config = configparser.ConfigParser()
    config.add_section("CSV_file")
    config.set("CSV_file", "Working", "True")
    config.set("CSV_file", "file name", "output.log")
    config.add_section("sieve_interval")
    config.set("sieve_interval", "sieve_interval", "100000")
    config.add_section("over_smooth")
    config.set("over_smooth", "over_smooth", "20")
    config.add_section("numbers")
    config.set("numbers", "numbers", "")

    with open(path, "w") as config_file:
        config.write(config_file)


def get_config(path):
    """
    :param path: path to file
    :return: the config object
    """
    if not os.path.exists(path):
        module_logger = logging.getLogger("sieve.get_config")
        module_logger.warning("Creating config file")
        try:
            create_config(path)
        except FileNotFoundError:
            os.makedirs(os.path.dirname(path))
            create_config(path)
        module_logger.warning("Exiting")
        exit()

    config = configparser.ConfigParser()
    config.read(path)
    return config


def get_setting(path, section, setting):
    """
    Print out settings
    :param path: path to file
    :param section: section of file
    :param setting: setting name
    :return: value of settings
    """
    config = get_config(path)
    value = config.get(section, setting)
    return value


if __name__ == '__main__':
    path = "configs/settings.ini"
    try:
        logging.config.fileConfig("configs/logging.conf")
    except:
        FORMAT = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        logging.basicConfig(level=logging.INFO, format=FORMAT, filename="sieve.log")
        stderrLogger = logging.StreamHandler()
        stderrLogger.setFormatter(logging.Formatter(FORMAT))
        logging.getLogger().addHandler(stderrLogger)

    logger = logging.getLogger("sieve")
    compile_program()
    fieldnames = ["Number", "Result", "Time"]
    logger.info("Getting numbers")

    if get_setting(path, "numbers", "numbers"):
        numbers = tuple(sys.argv[1:] + get_setting(path, "numbers", "numbers").split(' '))
    else:
        numbers = tuple(sys.argv[1:])

    # ThreadPoolExecutor executes on threads, has global lock
    # ProcessPoolExecutor executes on cores, doesn't have global lock
    with concurrent.futures.ProcessPoolExecutor() as executor:
        results = executor.map(sieve, numbers)

    if get_setting(path, 'CSV_file', 'Working') == 'True' or get_setting(path, 'CSV_file', 'Working') == 'true':
        csv_dict_writer(get_setting(path, 'CSV_file', 'file name'), fieldnames, results)
