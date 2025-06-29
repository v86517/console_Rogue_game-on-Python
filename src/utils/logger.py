from loguru import logger  # Импортируем библиотеку loguru для логирования

from utils.utils import get_project_root  # Импортируем функцию get_project_root из модуля utils.utils

logger.remove()  # Удаляем все предыдущие конфигурации логгера

log_directory = get_project_root() / "log"  # Создаем путь к директории для логов
log_directory.mkdir(exist_ok=True)  # Создаем директорию для логов, если она не существует

controller_log = logger.bind(name="controller")  # Создаем логгер для контроллера
controller_log.add(  # Добавляем конфигурацию для логгера контроллера
    log_directory / "controller.log",  # Указываем путь к файлу лога
    format=(  # Задаем формат логов
        "<green>[{time:YYYY-MM-DD HH:mm:ss}]</green> "
        "<cyan>{file}</cyan>:<cyan>{line}</cyan> "
        "<level>{level: <8}</level>: {message}"
    ),
    level="DEBUG",  # Устанавливаем уровень логирования на DEBUG
    filter=lambda record: record["extra"].get("name") == "controller",  # Фильтруем логи по имени "controller"
    rotation="10 MB",  # Устанавливаем ротацию логов при достижении размера 10 MB
    backtrace=True,  # Включаем трассировку стека
    diagnose=True,  # Включаем диагностику
)

view_log = logger.bind(name="view")  # Создаем логгер для представления
view_log.add(  # Добавляем конфигурацию для логгера представления
    log_directory / "view.log",  # Указываем путь к файлу лога
    format=(  # Задаем формат логов
        "<green>[{time:YYYY-MM-DD HH:mm:ss}]</green> "
        "<cyan>{file}</cyan>:<cyan>{line}</cyan> "
        "<level>{level: <8}</level>: {message}"
    ),
    level="DEBUG",  # Устанавливаем уровень логирования на DEBUG
    filter=lambda record: record["extra"].get("name") == "view",  # Фильтруем логи по имени "view"
    rotation="10 MB",  # Устанавливаем ротацию логов при достижении размера 10 MB
    backtrace=True,  # Включаем трассировку стека
    diagnose=True,  # Включаем диагностику
)

domain_log = logger.bind(name="domain")  # Создаем логгер для домена
domain_log.add(  # Добавляем конфигурацию для логгера домена
    log_directory / "domain.log",  # Указываем путь к файлу лога
    format=(  # Задаем формат логов
        "<green>[{time:YYYY-MM-DD HH:mm:ss}]</green> "
        "<cyan>{file}</cyan>:<cyan>{line}</cyan> "
        "<level>{level: <8}</level>: {message}"
    ),
    level="DEBUG",  # Устанавливаем уровень логирования на DEBUG
    filter=lambda record: record["extra"].get("name") == "domain",  # Фильтруем логи по имени "domain"
    rotation="10 MB",  # Устанавливаем ротацию логов при достижении размера 10 MB
    backtrace=True,  # Включаем трассировку стека
    diagnose=True,  # Включаем диагностику
)
