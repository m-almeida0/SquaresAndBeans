import matplotlib
matplotlib.use('Qt5Agg')
import matplotlib.pyplot as plt
import csv

generations = []
scores = []
survival_times = []
n_dodges = []
av_time = []
av_dodge = []
av_score = []

# Read data from CSV file
with open("csv.out") as csvfile:
    reader = csv.DictReader(csvfile)
    for row in reader:
        generations.append(int(row["generation"]))
        scores.append(float(row["score"]))
        survival_times.append(float(row["time"]))
        n_dodges.append(int(row["n dodges"]))
        av_time.append(int(row["av time"]))
        av_dodge.append(int(row["av dodge"]))
        av_score.append(int(row["av score"]))

# Create a figure with 3 rows and 1 column of subplots
fig, (ax1, ax2, ax3, ax4, ax5, ax6) = plt.subplots(6, 1, figsize=(8, 6))

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

ax4.plot(generations, av_score, color='skyblue', linestyle='-', label='Av Score')
ax4.set_xlabel("Generation")
ax4.set_ylabel("Av Score")
ax4.set_title("Generation vs. Av Score")
ax4.legend()
ax4.grid(True)

ax5.plot(generations, av_time, color='orange', linestyle='-', label='Av Time')
ax5.set_xlabel("Generation")
ax5.set_ylabel("Av Time")
ax5.set_title("Generation vs. Av Time")
ax5.legend()
ax5.grid(True)

ax6.plot(generations, av_dodge, color='green', linestyle='-', label='Av Dodge')
ax6.set_xlabel("Generation")
ax6.set_ylabel("Av Dodge")
ax6.set_title("Generation vs. Av Dodge")
ax6.legend()
ax6.grid(True)

# Adjust spacing between subplots
plt.tight_layout()

# Obtain the figure manager for maximizing
figManager = plt.get_current_fig_manager()

# Maximize the plot window
figManager.window.showMaximized()

# Show the combined plot
plt.show()
