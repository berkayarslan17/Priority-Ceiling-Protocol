# Priority Ceiling Protocol

### Requirements

1- ***(DONE)*** gtu::mutex shall implement std::mutex interface that is required by std::lock_guard
and std::unique_lock.

2- ***(DONE)*** gtu::mutex shall provide an interface to register threads that are capable of locking
this mutex in advance. During runtime, if a thread tries to lock the mutex, but hasn’t registered
itself before, a runtime exception shall be raised.

3- ***(DONE)*** The designed mutex shall be able to avoid deadlocks.

4- ***(IN PROGRESS)*** If a lock attempt is blocked due to priority ceiling protocol rule (that is,
the thread trying to lock does not have higher priority than the highest ceiling of all acquired
locks), the priority of the thread holding the lock will be elevated if necessary.

### Design

1- Create gtu::mutex members with a priority ceiling.

2- Create std::thread members.

3- Register each thread to each gtu::mutex member.

4- When one of the thread attempts to take lock of the mutex, control registration and
ceiling violation.

5- If the thread has not registered to mutex before, throw an exception.

6- If the thread’s priority is greater than mutex’s ceiling value, it takes the lock.

### Build and Test

![resim](https://user-images.githubusercontent.com/44584158/170895949-1edc0c4d-a9e7-4d04-87bf-344d28b39ea7.png)

1.Create a directory for keeping your build configuration: **mkdir -p out/build**

2.Create a directory for your source files: **mkdir src**

3.Compile & Run: **cd out/build && cmake -S ../../ -B . && make && ./mutex**

### Output

![resim](https://user-images.githubusercontent.com/44584158/170895632-d8d10396-a5a8-4785-a3a1-69595815baaf.png)


