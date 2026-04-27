from enum import Enum

class ProcessState(Enum):
    IDLE = 0
    LINEAR_ACTUATOR_DOWN = 1
    LINEAR_ACTUATOR_RUNNING = 2
    LINEAR_ACTUATOR_STOPPING = 3
    LINEAR_ACTUATOR_UP = 4
    MOTOR_MOVING_TO_CAM = 5
    PICTURE_TAKING = 6
    PICTURE_TAKING_DONE = 7
    RESETTING = 8
    HALTED = 9

def getProcessMessage(state: ProcessState):
    match state:
        case ProcessState.IDLE:
            return "Parçayı takıp devam et tuşuna basınız."
        case ProcessState.LINEAR_ACTUATOR_DOWN:
            return "Lineer Aktüatör çalışmaya başladı."
        case ProcessState.LINEAR_ACTUATOR_RUNNING:
            return "Malzeme çiziliyor..."
        case ProcessState.LINEAR_ACTUATOR_STOPPING:
            return "Malzeme çizimi tamamlandı!"
        case ProcessState.LINEAR_ACTUATOR_UP:
            return "Lineer Aktüatör sıfırlanıyor..."
        case ProcessState.MOTOR_MOVING_TO_CAM:
            return "Görüntü çekimi için kameraya yaklaşılıyor..."
        case ProcessState.PICTURE_TAKING:
            return "Görüntü çekimi başladı!"
        case ProcessState.PICTURE_TAKING_DONE:
            return "Görüntü çekimi tamamlandı!"
        case ProcessState.RESETTING:
            return "Sistem sıfırlanıyor..."
        case ProcessState.HALTED:
            return "Sistem hatası, lütfen tekrar başlatınız."
        case _:
            return "Sistem hatası!"
        
def getProcessButtonMessage(state: ProcessState):
    match state:
        case ProcessState.IDLE:
            return "Devam et ve İşlemi Başlat"
        case _:
            return "İŞLEM YÜRÜTÜLÜYOR"
