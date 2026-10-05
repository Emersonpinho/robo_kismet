import pyttsx3

motor = pyttsx3.init()
for v in motor.getProperty("voices"):
    print(v.id)
    print("  nome:", v.name)
    print("  idiomas:", v.languages)
    print()