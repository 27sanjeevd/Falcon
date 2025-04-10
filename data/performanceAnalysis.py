import pandas as pd
import matplotlib.pyplot as plt
from matplotlib.ticker import MaxNLocator, MultipleLocator

# Load data from CSV file
df = pd.read_csv('data/coinbase_map.csv', header=0)
values = df.iloc[:, 0].astype(int)

# Remove the largest value from the dataset
max_value = values.max()
values = values[values != max_value]

# Count frequencies and sort
value_counts = values.value_counts().sort_index()

new_index = []
new_value = []

for (a, b) in zip(value_counts.index, value_counts.values):
    if a < 45000:
        new_index.append(a)
        new_value.append(b)

# Create plot with real numbers on x-axis
plt.figure(figsize=(12, 6))
plt.bar(new_index,
        new_value,
        width=25,
        align='center',
        color='#1f77b4',
        edgecolor='black',
        alpha=0.7)

# Formatting
plt.title('Latency Frequency Distribution', pad=20, fontsize=14)
plt.xlabel('Latency (ns)', fontsize=12)
plt.ylabel('Frequency', fontsize=12)
plt.grid(axis='y', alpha=0.3)

# Configure axes to avoid scientific notation and improve readability
ax = plt.gca()
ax.xaxis.set_major_formatter(plt.FuncFormatter(lambda x, _: f'{int(x)}'))
ax.xaxis.set_major_locator(MultipleLocator(5000))  # Reduce number of ticks dynamically
ax.tick_params(axis='x', labelsize=8)  # Adjust font size for clarity

plt.xticks(rotation=45, ha='right')  # Rotate labels for better readability

plt.tight_layout()
plt.show()
