import csv
import matplotlib.pyplot as plt

bids = []
asks = []

def insert_order(order_list, price, is_bid=True):
    if is_bid:
        for x in range(len(order_list)):
            if order_list[x] == price:
                return x
            elif order_list[x] < price:
                order_list.insert(x, price)
                return x
    else:
        for x in range(len(order_list)):
            if order_list[x] == price:
                return x
            elif order_list[x] > price:
                order_list.insert(x, price)
                return x
            
    order_list.append(price)
    return len(order_list)


def remove_order(order_list, price):
    for x in range(len(order_list)):
        if order_list[x] == price:
            return x
        
    return -1


def process_orderbook(file_path):
    level_change = {}
    deletions = {}

    with open(file_path, newline='') as csvfile:
        reader = csv.reader(csvfile)
        next(reader)

        for price, volume, side in reader:
            price = float(price)
            volume = float(volume)

            orderbook = asks if side == "offer" else bids
            is_bid = False if side == "offer" else True

            if volume != 0.0:
                temp_index = insert_order(orderbook, price, is_bid)
                if temp_index not in level_change:
                    level_change[temp_index] = 0

                level_change[temp_index] += 1

            else:
                temp_index = remove_order(orderbook, price)
                if temp_index not in deletions:
                    deletions[temp_index] = 0

                deletions[temp_index] += 1

    return level_change, deletions

if __name__ == "__main__":
    file_path = "data/coinbase_btc.csv"
    level_change, deletions = process_orderbook(file_path)
    
    if -1 in deletions:
        del deletions[-1]

    #print(deletions)

    """
    print("Level Change Counts:")
    for level, count in sorted(level_change_count.items()):
        print(f"Level {level}: {count} changes")

    print("\nLevel Deletion Counts:")
    for level, count in sorted(level_deletion_count.items()):
        print(f"Level {level}: {count} deletions")
    """


    keys = list(level_change.keys())
    values = list(level_change.values())

    # Create the bar plot
    plt.figure(figsize=(10, 6))
    plt.bar(keys, values)

    # Add labels and title
    plt.xlabel('Insert Depth')
    plt.ylabel('Occurences')
    plt.title('Bar Plot of Coinbase BTC Insertion/Change Data')

    # Rotate x-axis labels for better visibility
    plt.xticks(rotation=90)

    # Show plot
    plt.tight_layout()
    plt.show()
