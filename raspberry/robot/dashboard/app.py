"""
Dashboard web para monitoreo y control remoto del Robot Móvil Autónomo
"""

# Type checking suppressions for dynamic libraries
# type: ignore

from typing import Any, Dict, List, Optional, Union
import streamlit as st  # type: ignore
import time
import numpy as np
import plotly.graph_objects as go  # type: ignore
import plotly.express as px  # type: ignore
import pandas as pd  # type: ignore
from datetime import datetime, timedelta

# Configuración de la página
st.set_page_config(
    page_title="AMR Dashboard", 
    page_icon="🤖", 
    layout="wide",
    initial_sidebar_state="expanded"
)

# CSS personalizado para mejorar el aspecto visual
st.markdown("""
<style>
    .main-header {
        background: linear-gradient(90deg, #1f4e79 0%, #2980b9 100%);
        padding: 1rem;
        border-radius: 10px;
        margin-bottom: 2rem;
    }
    .main-header h1 {
        color: white;
        text-align: center;
        margin: 0;
        font-size: 2.5rem;
    }
    .status-card {
        background: #f8f9fa;
        padding: 1rem;
        border-radius: 8px;
        border-left: 4px solid #2980b9;
        margin: 0.5rem 0;
    }
    .metric-container {
        background: white;
        padding: 1rem;
        border-radius: 8px;
        box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        text-align: center;
    }
    .control-button {
        width: 100%;
        height: 60px;
        font-size: 1.2rem;
        margin: 0.2rem 0;
    }
    .sidebar-section {
        background: #f1f3f4;
        padding: 1rem;
        border-radius: 8px;
        margin: 1rem 0;
    }
</style>
""", unsafe_allow_html=True)

# Encabezado principal
st.markdown("""
<div class="main-header">
    <h1>🤖 Dashboard - Robot Móvil Autónomo</h1>
</div>
""", unsafe_allow_html=True)

# Inicializar estado de emergencia en session_state si no existe
if 'emergency_stop' not in st.session_state:
    st.session_state.emergency_stop = False

# Sidebar para configuración y estado
with st.sidebar:
    st.markdown("### ⚙️ Configuración")
    auto_refresh = st.checkbox("Actualización automática", value=True)
    refresh_rate = st.slider("Tasa de actualización (seg)", 1, 10, 2)
    
    st.markdown("### 🔧 Modo de operación")
    # Deshabilitar selección de modo si hay parada de emergencia
    if st.session_state.emergency_stop:
        operation_mode = st.selectbox(
            "Seleccionar modo:",
            ["Manual", "Autónomo", "Patrulla", "Mapeo"],
            disabled=True,
            help="Modo deshabilitado - Parada de emergencia activa"
        )
    else:
        operation_mode = st.selectbox(
            "Seleccionar modo:",
            ["Manual", "Autónomo", "Patrulla", "Mapeo"]
        )
    
    st.markdown("### 📊 Estadísticas del sistema")
    uptime = st.metric("Tiempo activo", "2h 34m")
    
    # Sistema de parada de emergencia mejorado
    st.markdown("### 🚨 Control de Emergencia")
    
    if not st.session_state.emergency_stop:
        # Botón para activar parada de emergencia
        if st.button("🚨 PARADA DE EMERGENCIA", type="primary", help="Detiene inmediatamente todos los sistemas"):
            st.session_state.emergency_stop = True
            st.rerun()
    else:
        # Estado de emergencia activo
        st.error("🚨 **PARADA DE EMERGENCIA ACTIVA**")
        st.markdown("**Todos los sistemas están detenidos**")
        
        # Botón para reactivar sistema
        col1, col2 = st.columns(2)
        with col1:
            if st.button("🔄 REACTIVAR", type="secondary", help="Reactiva el sistema después de verificar la seguridad"):
                st.session_state.emergency_stop = False
                st.success("✅ Sistema reactivado correctamente")
                st.rerun()
        with col2:
            if st.button("ℹ️ Estado", help="Ver detalles del estado de emergencia"):
                st.info("Verifique que el área esté despejada antes de reactivar")

# Funciones auxiliares
def get_robot_status(operation_mode: str, emergency_stop: bool) -> Dict[str, Any]:
    """Obtiene el estado actual del robot (simulado)"""
    current_time = datetime.now()
    
    # Inicializar posición base si no existe
    if 'robot_base_position' not in st.session_state:
        st.session_state.robot_base_position = {
            'x': 5.0,
            'y': 5.0, 
            'theta': 180.0
        }
    
    # Generar pequeños cambios graduales en lugar de valores completamente aleatorios
    base_pos = st.session_state.robot_base_position
    
    # Actualizar posición base ocasionalmente (10% de probabilidad)
    if np.random.random() < 0.1:
        st.session_state.robot_base_position['x'] += np.random.uniform(-0.2, 0.2)
        st.session_state.robot_base_position['y'] += np.random.uniform(-0.2, 0.2)
        st.session_state.robot_base_position['theta'] += np.random.uniform(-5, 5)
        st.session_state.robot_base_position['theta'] %= 360
    
    return {
        'timestamp': current_time,
        'position': {
            'x': round(base_pos['x'] + np.random.uniform(-0.1, 0.1), 2),
            'y': round(base_pos['y'] + np.random.uniform(-0.1, 0.1), 2),
            'theta': round((base_pos['theta'] + np.random.uniform(-2, 2)) % 360, 1)
        },
        'sensors': {
            'lidar': [round(np.random.uniform(0.1, 3.0), 2) for _ in range(8)],
            'ultrasonic_front': round(np.random.uniform(0.05, 2.0), 2),
            'ultrasonic_back': round(np.random.uniform(0.05, 2.0), 2),
            'imu': {
                'accel_x': round(np.random.uniform(-1, 1), 3),
                'accel_y': round(np.random.uniform(-1, 1), 3),
                'accel_z': round(np.random.uniform(9.5, 10.5), 3),
                'gyro_z': round(np.random.uniform(-5, 5), 3)
            }
        },
        'motors': {
            'left_speed': round(np.random.uniform(0, 100), 1),
            'right_speed': round(np.random.uniform(0, 100), 1),
            'left_encoder': int(np.random.randint(0, 10000)),
            'right_encoder': int(np.random.randint(0, 10000))
        },
        'battery': {
            'voltage': round(np.random.uniform(11.5, 12.6), 2),
            'percentage': int(np.random.randint(20, 100))
        },
        'status': "🔴 EMERGENCIA" if emergency_stop else np.random.choice(["🟢 Operativo", "🟡 Advertencia", "🔴 Error"], p=[0.7, 0.2, 0.1]),
        'mode': "PARADA DE EMERGENCIA" if emergency_stop else operation_mode,
        'emergency_active': emergency_stop
    }

def create_lidar_plot(lidar_data: List[float]) -> Any:  # type: ignore
    """Crea un gráfico polar para los datos del LIDAR"""
    angles = np.linspace(0, 2*np.pi, len(lidar_data), endpoint=False)
    angles_deg = np.degrees(angles)
    
    fig = go.Figure()
    
    fig.add_trace(go.Scatterpolar(
        r=lidar_data,
        theta=angles_deg,
        mode='lines+markers',
        name='LIDAR',
        line=dict(color='#2980b9', width=2),
        marker=dict(size=6)
    ))
    
    fig.update_layout(
        polar=dict(
            radialaxis=dict(
                visible=True,
                range=[0, 3],
                title="Distancia (m)"
            ),
            angularaxis=dict(
                direction="counterclockwise",
                period=360
            )
        ),
        title="Vista LIDAR 360°",
        height=400,
        margin=dict(l=50, r=50, t=50, b=50)
    )
    
    return fig

def create_trajectory_plot(current_pos: Dict[str, float]) -> Any:  # type: ignore
    """Crea un gráfico de trayectoria del robot"""
    # Generar trayectoria simulada
    time_points = np.linspace(0, 60, 50)  # 60 segundos de historia
    x_history = current_pos['x'] + np.cumsum(np.random.normal(0, 0.1, 50))
    y_history = current_pos['y'] + np.cumsum(np.random.normal(0, 0.1, 50))
    
    fig = go.Figure()
    
    # Trayectoria
    fig.add_trace(go.Scatter(
        x=x_history[:-1],
        y=y_history[:-1],
        mode='lines',
        name='Trayectoria',
        line=dict(color='#3498db', width=2)
    ))
    
    # Posición actual
    fig.add_trace(go.Scatter(
        x=[current_pos['x']],
        y=[current_pos['y']],
        mode='markers',
        name='Posición actual',
        marker=dict(
            size=15,
            color='#e74c3c',
            symbol='arrow-up',
            angle=current_pos['theta']
        )
    ))
    
    fig.update_layout(
        title="Trayectoria del Robot",
        xaxis_title="X (m)",
        yaxis_title="Y (m)",
        height=400,
        margin=dict(l=50, r=50, t=50, b=50),
        showlegend=True
    )
    
    return fig

# Obtener datos del robot
robot_data = get_robot_status(operation_mode, st.session_state.emergency_stop)

# === SECCIÓN PRINCIPAL: ESTADO DEL ROBOT ===
st.markdown("## 📊 Estado del Sistema")

# Primera fila: Métricas principales
col1, col2, col3, col4 = st.columns(4)

with col1:
    st.metric(
        label="🔋 Batería", 
        value=f"{robot_data['battery']['percentage']}%",
        delta=f"{robot_data['battery']['voltage']}V"
    )

with col2:
    st.metric(
        label="📍 Posición X", 
        value=f"{robot_data['position']['x']:.2f} m",
        delta=None
    )

with col3:
    st.metric(
        label="📍 Posición Y", 
        value=f"{robot_data['position']['y']:.2f} m",
        delta=None
    )

with col4:
    st.metric(
        label="🧭 Orientación", 
        value=f"{robot_data['position']['theta']:.1f}°",
        delta=None
    )

# Segunda fila: Estado y modo
col_status, col_mode = st.columns(2)
with col_status:
    if robot_data['emergency_active']:
        st.error(f"**Estado:** {robot_data['status']}")
    elif "🔴" in robot_data['status']:
        st.error(f"**Estado:** {robot_data['status']}")
    elif "🟡" in robot_data['status']:
        st.warning(f"**Estado:** {robot_data['status']}")
    else:
        st.success(f"**Estado:** {robot_data['status']}")

with col_mode:
    if robot_data['emergency_active']:
        st.error(f"**Modo:** {robot_data['mode']}")
    else:
        st.info(f"**Modo:** {robot_data['mode']}")

# Alerta prominente de emergencia si está activa
if robot_data['emergency_active']:
    st.markdown("""
    <div style="
        background: linear-gradient(90deg, #e74c3c, #c0392b);
        color: white;
        padding: 1rem;
        border-radius: 10px;
        text-align: center;
        font-size: 1.2rem;
        font-weight: bold;
        margin: 1rem 0;
        border: 3px solid #a93226;
        box-shadow: 0 4px 8px rgba(0,0,0,0.3);
    ">
        🚨 PARADA DE EMERGENCIA ACTIVA 🚨<br>
        <small>Todos los sistemas del robot están detenidos por seguridad</small>
    </div>
    """, unsafe_allow_html=True)

# === SECCIÓN: VISUALIZACIÓN DE SENSORES ===
st.markdown("## 🎯 Sensores y Navegación")

col_lidar, col_trajectory = st.columns(2)

with col_lidar:
    try:
        lidar_fig = create_lidar_plot(robot_data['sensors']['lidar'])
        st.plotly_chart(lidar_fig, use_container_width=True)
    except:
        st.error("Error al cargar gráfico LIDAR - Se requiere instalación de plotly")
        st.write("**Datos LIDAR:**", robot_data['sensors']['lidar'])

with col_trajectory:
    try:
        traj_fig = create_trajectory_plot(robot_data['position'])
        st.plotly_chart(traj_fig, use_container_width=True)
    except:
        st.error("Error al cargar gráfico de trayectoria - Se requiere instalación de plotly")
        st.write(f"**Posición actual:** X={robot_data['position']['x']:.2f}m, Y={robot_data['position']['y']:.2f}m")

# === SECCIÓN: DATOS DETALLADOS ===
st.markdown("## 🔬 Datos Detallados")

tab1, tab2, tab3 = st.tabs(["🚁 Sensores", "⚙️ Motores", "📱 IMU"])

with tab1:
    col_ultra1, col_ultra2 = st.columns(2)
    with col_ultra1:
        st.metric("🔍 Ultrasónico Frontal", f"{robot_data['sensors']['ultrasonic_front']} m")
    with col_ultra2:
        st.metric("🔍 Ultrasónico Trasero", f"{robot_data['sensors']['ultrasonic_back']} m")

with tab2:
    col_motor1, col_motor2 = st.columns(2)
    with col_motor1:
        st.metric("🔄 Motor Izquierdo", f"{robot_data['motors']['left_speed']}%")
        st.write(f"Encoder: {robot_data['motors']['left_encoder']} pulsos")
    with col_motor2:
        st.metric("🔄 Motor Derecho", f"{robot_data['motors']['right_speed']}%")
        st.write(f"Encoder: {robot_data['motors']['right_encoder']} pulsos")

with tab3:
    col_accel, col_gyro = st.columns(2)
    with col_accel:
        st.write("**Acelerómetro:**")
        st.write(f"• X: {robot_data['sensors']['imu']['accel_x']} m/s²")
        st.write(f"• Y: {robot_data['sensors']['imu']['accel_y']} m/s²")
        st.write(f"• Z: {robot_data['sensors']['imu']['accel_z']} m/s²")
    with col_gyro:
        st.write("**Giroscopio:**")
        st.write(f"• Z: {robot_data['sensors']['imu']['gyro_z']} °/s")

# === SECCIÓN: CONTROL MANUAL ===
st.markdown("## 🎮 Control Manual")

# Verificar estado de emergencia
if st.session_state.emergency_stop:
    st.error("🚨 **CONTROLES DESHABILITADOS** - Parada de emergencia activa")
    st.markdown("Todos los controles están bloqueados por seguridad. Reactivar el sistema desde el panel lateral.")
    
elif operation_mode == "Manual":
    control_col1, control_col2, control_col3 = st.columns([1, 2, 1])
    
    with control_col2:
        # Botón adelante
        if st.button("⬆️ ADELANTE", key="forward", help="Mover hacia adelante"):
            st.success("✅ Comando enviado: Adelante")
        
        # Fila de controles laterales
        left_col, stop_col, right_col = st.columns(3)
        
        with left_col:
            if st.button("⬅️ IZQUIERDA", key="left", help="Girar a la izquierda"):
                st.success("✅ Comando enviado: Izquierda")
        
        with stop_col:
            if st.button("⏹️ DETENER", key="stop", help="Detener todos los motores"):
                st.warning("⚠️ Robot detenido")
        
        with right_col:
            if st.button("➡️ DERECHA", key="right", help="Girar a la derecha"):
                st.success("✅ Comando enviado: Derecha")
        
        # Botón atrás
        if st.button("⬇️ ATRÁS", key="backward", help="Mover hacia atrás"):
            st.success("✅ Comando enviado: Atrás")
    
    # Control de velocidad
    with control_col1:
        st.markdown("### ⚡ Control de Velocidad")
        speed = st.slider("Velocidad (%)", 0, 100, 50)
        st.write(f"Velocidad actual: {speed}%")
    
    with control_col3:
        st.markdown("### 🎚️ Controles Avanzados")
        if st.button("🔄 Rotación 90°", key="rotate_90"):
            st.info("Ejecutando rotación...")
        if st.button("📏 Avanzar 1m", key="advance_1m"):
            st.info("Avanzando 1 metro...")

else:
    st.info(f"🤖 Robot en modo **{operation_mode}**. Los controles manuales están deshabilitados.")

# === SECCIÓN: VISTA DE CÁMARA ===
st.markdown("## 📷 Vista de Cámara")
col_cam1, col_cam2 = st.columns(2)

with col_cam1:
    st.markdown("### 📹 Cámara Principal")
    # Placeholder para cámara real
    st.image("https://via.placeholder.com/400x300/2980b9/ffffff?text=Cámara+Principal", 
             caption="Vista frontal del robot")

with col_cam2:
    st.markdown("### 🗺️ Mapa Local")
    # Placeholder para mapa
    st.image("https://via.placeholder.com/400x300/27ae60/ffffff?text=Mapa+de+Navegación", 
             caption="Mapa generado por SLAM")

# === SECCIÓN: LOGS Y EVENTOS ===
st.markdown("## 📝 Registro de Eventos")

# Generar logs simulados
current_time = datetime.now()
logs_data = []
for i in range(10):
    timestamp = current_time - timedelta(minutes=i*2)
    event_types = ["INFO", "WARNING", "SUCCESS", "DEBUG"]
    event_type = np.random.choice(event_types, p=[0.5, 0.2, 0.2, 0.1])
    
    messages = {
        "INFO": ["Sistema iniciado correctamente", "Datos de sensores actualizados", "Posición actualizada"],
        "WARNING": ["Batería baja detectada", "Obstáculo cercano", "Señal débil de GPS"],
        "SUCCESS": ["Comando ejecutado exitosamente", "Navegación completada", "Calibración finalizada"],
        "DEBUG": ["Depuración de motores", "Verificación de sensores", "Test de comunicación"]
    }
    
    message = np.random.choice(messages[event_type])
    logs_data.append({
        "Timestamp": timestamp.strftime("%H:%M:%S"),
        "Tipo": event_type,
        "Mensaje": message
    })

# Mostrar logs en una tabla
logs_df = pd.DataFrame(logs_data)
st.dataframe(logs_df, use_container_width=True, hide_index=True)

# === ACTUALIZACIÓN AUTOMÁTICA ===
if auto_refresh:
    st.info(f"🔄 Actualización automática activa (cada {refresh_rate}s)")
    
    # Método simple y funcional: sleep + rerun
    # Este enfoque funciona bien para dashboards en tiempo real
    time.sleep(refresh_rate)
    st.rerun()
    
else:
    st.info("⏸️ Actualización automática desactivada")

# Botón de actualización manual
st.markdown("---")
col_refresh1, col_refresh2, col_refresh3 = st.columns([1, 1, 1])
with col_refresh2:
    if st.button("🔄 Actualizar Datos", key="manual_update", help="Actualizar datos manualmente", use_container_width=True):
        st.success("¡Datos actualizados!")
        st.rerun()