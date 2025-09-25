"""
Punto de entrada principal para Streamlit Cloud
"""
import sys
import os

# Agregar el directorio del dashboard al path
sys.path.append(os.path.join(os.path.dirname(__file__), 'raspberry', 'robot', 'dashboard'))

# Importar y ejecutar la aplicación
from raspberry.robot.dashboard.app import *