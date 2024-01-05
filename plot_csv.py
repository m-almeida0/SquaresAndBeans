import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import csv

generations = []
scores = []
survival_times = []
n_dodges = []

# Read data from CSV file
with open("csv.out") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        generations.append(int(row["generation"]))
        scores.append(float(row["score"]))
        survival_times.append(float(row["time"]))
        n_dodges.append(int(row["n dodges"]))

# Create a figure with 3 rows and 1 column of subplots
fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(8, 6))

# Plot each graph on its respective subplot
ax1.plot(generations, scores, color='skyblue', linestyle='-', label='Score')
ax1.set_xlabel("Generation")
ax1.set_ylabel("Score")
ax1.set_title("Generation vs. Score")
ax1.legend()
ax1.grid(True)

ax2.plot(generations, survival_times, color='orange', linestyle='-', label='Survival Time')
ax2.set_xlabel("Generation")
ax2.set_ylabel("Survival Time")
ax2.set_title("Generation vs. Survival Time")
ax2.legend()
ax2.grid(True)

ax3.plot(generations, n_dodges, color='green', linestyle='-', label='N Dodges')
ax3.set_xlabel("Generation")
ax3.set_ylabel("N Dodges")
ax3.set_title("Generation vs. N Dodges")
ax3.legend()
ax3.grid(True)

# Adjust spacing between subplots
plt.tight_layout()

# Obtain the figure manager for maximizing
figManager = plt.get_current_fig_manager()

# Maximize the plot window
figManager.window.showMaximized()

# Show the combined plot
plt.show()
