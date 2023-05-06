from prettytable import PrettyTable
import pandas


def show_status_tables(caches):
    n = len(caches)
    # tables = list()
    for i in range(n):
        status_table = PrettyTable()
        status_table.field_names = ["block_addr", "status", "mem_addr", "data"]
        status_table.add_rows(caches[i].get_status_rows())
        status_table.title = "cache" + str(i)
        print(status_table)
        # tables.append(status_table)
    # print(tables)


def show_status_tables_df(caches):
    n = len(caches)
    for i in range(n):
        df = pandas.DataFrame(caches[i].get_status_rows(), columns=["block_addr", "status", "mem_addr", "data"])
        print(df)
