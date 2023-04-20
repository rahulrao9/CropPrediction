# Code to train the tinyML model
import numpy as np
import pandas as pd
from everywhereml.sklearn.ensemble import RandomForestClassifier
from sklearn.model_selection import train_test_split
from sklearn.metrics import accuracy_score

crop_data_df = pd.read_csv('Crop_recommendation.csv')

X = crop_data_df.drop(
    columns=['N', 'P', 'K', 'ph', 'rainfall', 'label'], axis=1)
print(X)
y = pd.Categorical(crop_data_df['label']).codes

X_train, X_test, y_train, y_test = train_test_split(
    X, y, test_size=0.33, shuffle=True, random_state=0)

model = RandomForestClassifier(n_estimators=10, criterion='entropy')

model.fit(X_train, y_train)

y_pred = model.predict(X_test)

print(y_pred)

print('Random Forest Classifier -TinyML accuracy ' +
      str(accuracy_score(y_test, y_pred)))

model.to_arduino_file('CropClassifierModel.h', instance_name = 'cropClassifier')
