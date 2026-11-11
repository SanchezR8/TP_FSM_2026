# TP_FSM_2026
Control de llenado de tanque de agua

# Control Automático de Llenado de Tanque

**Materia:** Informática II

---

## 1. ¿Qué hace el proyecto?

El objetivo de este proyecto es controlar el llenado y vaciado de un tanque de agua de manera completamente automática y segura. 

El sistema monitorea constantemente el nivel de líquido y el estado de los componentes para:
* Encender la bomba cuando el tanque se está quedando sin agua.
* Detener el llenado al llegar a la capacidad deseada.
* Detener todo y pasar a un modo de seguridad si detecta un sobrellenado, una falla mecánica o un paro manual.
* Permitir un vaciado controlado o reanudar el trabajo una vez resuelto cualquier problema.

La lógica está organizada mediante una **Máquina de Estados Finitos (FSM)** implementada con **punteros a función**, lo que hace que el flujo sea ordenado, modular y fácil de mantener.

## 2. Diagrama de Estados

<img width="1431" height="1099" alt="Diagrama FSM de control de tanque" src="https://github.com/user-attachments/assets/4c9e15b6-7530-4357-a129-4f1eddebda66" />

## 3. ¿Cómo funciona la Máquina de Estados?

El comportamiento del sistema se divide en 6 estados definidos según las condiciones del tanque:

* **0. INICIO:**  
  Al encenderse, el sistema lee los sensores iniciales. Si no hay fallas y el nivel es normal, pasa a **ESPERA**. Si el nivel ya está bajo ($\le 25\%$), puede iniciar el ciclo pasando a **LLENADO**.

* **1. ESPERA:**  
  Es el estado de reposo normal (bomba y válvula apagadas). Mientras el nivel sea mayor al $25\%$, permanece aquí. Si el nivel cae por debajo o igual al $25\%$, pasa a **LLENADO**. Si ocurre una falla o sobrellenado ($\ge 95\%$), pasa a **ALARMA**.

* **2. LLENADO:**  
  Se enciende la bomba de carga. El sistema se queda llenando mientras el nivel sea menor al $90\%$. Una vez alcanzado ese valor, apaga la bomba y vuelve a **ESPERA**. Si durante la carga ocurre una falla o sobrellenado, pasa de inmediato a **ALARMA**.

* **3. ALARMA:**  
  Modo de protección. Desactiva inmediatamente la bomba y la válvula para evitar accidentes y enciende la señal de alerta. Guarda en memoria qué estaba haciendo el sistema antes del fallo. Permanece en este estado hasta que se resuelva la causa y se presione el botón de **Reset**, pasando a **RECUPERACIÓN**.

* **4. RECUPERACIÓN:**  
  Permite decidir qué hacer tras salir de la alarma de forma segura:
  * **Reanudar llenado:** Si la alarma ocurrió mientras llenaba y todavía falta agua, retoma en **LLENADO**.
  * **Reanudar operación normal:** Vuelve a **ESPERA**.
  * **Seleccionar vaciado:** Si se decide drenar el tanque, transiciona a **VACIADO**.

* **5. VACIADO:**  
  Abre la válvula de desagüe (con la bomba apagada) para drenar el líquido. Permanece aquí mientras el nivel sea mayor a $0\%$. Cuando el tanque queda totalmente vacío ($0\%$), cierra la válvula y regresa a **ESPERA**.

