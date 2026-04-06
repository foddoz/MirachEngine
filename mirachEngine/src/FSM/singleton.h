#ifndef SINGLETON_H
#define SINGLETON_H

/**
 * @brief A generic Singleton template class.
 *
 * This template provides a thread-safe and lazy-initialized singleton instance.
 * The constructor and assignment operators are deleted to enforce a single instance.
 *
 * @tparam T The type of the singleton class.
 */
template <class T>
class Singleton 
{
public:
    /**
     * @brief Returns a pointer to the singleton instance.
     * The instance is created the first time this method is called and destroyed automatically
     * when the program terminates.
     * @return A pointer to the single instance of type T.
     */
    static T* Instance() 
    {
        static T instance;
        return &instance;
    }

private:
    /**
     * @brief Default constructor (private to prevent external instantiation).
     */
    Singleton() {}

    /**
     * @brief Destructor (private to prevent external deletion).
     */
    ~Singleton() {}

    /**
     * @brief Copy constructor is deleted to prevent copying.
     */
    Singleton(const Singleton&) = delete;

    /**
     * @brief Copy assignment operator is deleted to prevent assignment.
     */
    Singleton& operator=(const Singleton&) = delete;
};

#endif // SINGLETON_H
