import concurrent.futures
import subprocess
import logging
import datetime
import csv
import configparser
import os
import sys


def create_file():
    """
    Function creates file
    :return: None
    """
    module_logger = logging.getLogger("sieve.create_file")
    try:
        module_logger.info("Trying to create file with CMake and Make")
        subprocess.run(["cmake", "CMakeLists.txt"], check=True, stdout=subprocess.DEVNULL)
        subprocess.run("make", check=True, stdout=subprocess.DEVNULL)
    except subprocess.CalledProcessError:
        module_logger.warning("CMake or Make failed, trying with regular compilation")
        try:
            subprocess.run(["gcc", "list.c", "main.c", "-lm", "-lgmp", "-o", "quadratic-sieve"], check=True)
        except subprocess.CalledProcessError:
            module_logger.critical("GCC failed, exiting")
            exit()


def start(number):
    """
    Function starts sieve with chosen number
    :param number: number to sieve
    :return: dictionary of results and corresponding fieldname values
    """
    module_logger = logging.getLogger("sieve.start")
    module_logger.info("Starting sieve")
    before = datetime.datetime.now()
    sieve_result = None
    try:
        sieve_result = subprocess.run(["./quadratic-sieve", str(number)], capture_output=True, check=True)
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


if __name__ == '__main__':
    FORMAT = "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
    logging.basicConfig(level=logging.INFO, format=FORMAT)
    logger = logging.getLogger("sieve")
    create_file()
    fieldnames = ["Number", "Result", "Time"]
    numbers = (18446744073709551617, 184467440737095516170, 1844674407370955161700)

    # ThreadPoolExecutor wykonuje na wątkach, ale wykonuje global lock na wątku
    # ProcessPoolExecutor wykonuje na rdzeniach, nie ma global lock
    with concurrent.futures.ProcessPoolExecutor() as executor:
        results = executor.map(start, numbers)

    csv_dict_writer("output.csv", fieldnames, results)
