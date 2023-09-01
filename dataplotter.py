import matplotlib.pyplot as plt
import pandas as pd
import matplotlib.ticker as ticker
from scipy.stats import norm  # for PDF calculation
import numpy as np


# Read data from CSV
def calcStats(df):
    weighted_mean = (df['Moves'] * df['Frequencies']).sum() / df['Frequencies'].sum()
    weighted_std = ((df['Frequencies'] * (df['Moves'] - weighted_mean) ** 2).sum() / df['Frequencies'].sum()) ** 0.5
    print(df["Frequencies"].sum(), "\n\n\n")
    return weighted_mean, weighted_std


Meetdata = pd.read_csv('NoB_NoD_Meet.csv')
avgM, stdM = calcStats(Meetdata)
Crossdata = pd.read_csv("NoB_NoD_Cross.csv")
avgC, stdC = calcStats(Crossdata)
plt.scatter(Meetdata['Moves'], Meetdata['Frequencies'], label=f"To Meet, mean = {avgM}, std = {stdM} ", s=10)
plt.scatter(Crossdata['Moves'], Crossdata['Frequencies'], label=f"To Cross, mean = {avgC}, std = {stdC} ", s=10)
plt.xlabel('Moves')
plt.ylabel('Frequencies')
plt.title('Moves and Frequency for Meeting (n=200k) and Crossing (n=100k)')
plt.legend()
plt.xticks(rotation=90, fontsize=5)  # Rotate x-axis labels by 45 degrees
plt.subplots_adjust(bottom=0.70)
plt.gca().xaxis.set_major_locator(ticker.MultipleLocator(base=5))
plt.tight_layout()
plt.show()
