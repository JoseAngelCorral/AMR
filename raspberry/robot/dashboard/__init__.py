# type: ignore file for streamlit dashboard
# This file tells the Python type checker to ignore certain import issues

# Streamlit types are dynamically loaded, so we add type comments
from typing import Any, Dict, List, Optional, Union
import sys

# This helps with streamlit type checking
if 'streamlit' not in sys.modules:
    try:
        import streamlit
    except ImportError:
        pass

# Add type stubs for better IDE support
st: Any
go: Any  # plotly.graph_objects
px: Any  # plotly.express
pd: Any  # pandas