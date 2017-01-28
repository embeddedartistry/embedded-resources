/**
* Example code for:
*   - callback storage with std::vector
*   - use of std::function
*   - use of std::bind
*   - use of lambda function as callback
*/

#include <cstdint>
#include <cstdio>
#include <functional>
#include <vector>

/**
* Basic std::function callback example. Templated on a function signature
* that takes in a uint32_t and returns void.
*/
typedef std::function<void(uint32_t)> cb_t;

/**
* Here's an example type that stores a std::function and an argument
* to pass with that callback.  Passing a uint32_t input to your callback
* is a pretty common implementation. Also useful if you need to store a
* pointer for use with a BOUNCE function.
*/
struct cb_arg_t {
    //the callback - takes a uint32_t input.
    std::function<void(uint32_t)> cb;
    //value to return with the callback.
    uint32_t arg;
};

/**
* Alternative storage implementation.  Perhaps you want to store
* callbacks for different event types?
*/
enum my_events_t
{
    VIDEO_STOP = 0,
    VIDEO_START,
    EVENT_MAX
};

struct cb_event_t {
    std::function<void(uint32_t)> cb;
    my_events_t event;
};

/**
* Basic example.  Constructed with a uint32_t.
* Callbacks are passed this uint32_t.
*/
class BasicDriver {
public:

    BasicDriver(uint32_t val) : val_(val), callbacks_() { }

    // Register a callback.
    void register_callback(const cb_t &cb)
    {
        // add callback to end of callback list
        callbacks_.push_back(cb);
    }

    /// Call all the registered callbacks.
    void callback() const
    {
        // iterate through callback list and call each one
        for (const auto &cb : callbacks_)
        {
            cb(val_);
        }
    }

private:
    /// Integer to pass to callbacks.
    uint32_t val_;
    /// List of callback functions.
    std::vector<cb_t> callbacks_;
};

/**
* Event based example.  Constructed with a uint32_t.
* Callbacks are passed this uint32_t.
* Callbacks are only invoked when their event type matches the
* occuring event.
*/
class EventDriver {
public:

    EventDriver(uint32_t val) : val_(val), callbacks_() { }

    // Register a callback.
    void register_callback(const cb_t &cb, const my_events_t event)
    {
        // add callback to end of callback list
        callbacks_.push_back({cb, event});
    }

    /// Call all the registered callbacks.
    void callback() const
    {
        my_events_t event = VIDEO_START;
        // iterate through callback list and call each one
        for (const auto &cb : callbacks_)
        {
            if(cb.event == event)
            {
                cb.cb(val_);
            }
        }
    }

private:
    /// Integer to pass to callbacks.
    uint32_t val_;
    /// List of callback functions.
    std::vector<cb_event_t> callbacks_;
};

/**
* Arg based example.
* Callbacks register with a uint32_t that they want returned
* Callbacks always passed their specific uint32_t value
*/
class ArgDriver {
public:

    ArgDriver() : callbacks_() { }

    // Register a callback.
    void register_callback(const cb_t &cb, const uint32_t val)
    {
        // add callback to end of callback list
        callbacks_.push_back({cb, val});
    }

    /// Call all the registered callbacks.
    void callback() const
    {
        // iterate through callback list and call each one
        for (const auto &cb : callbacks_)
        {
            cb.cb(cb.arg);
        }
    }

private:
    /// List of callback functions.
    std::vector<cb_arg_t> callbacks_;
};


/**
* Dummy Client #1
* Uses a static method for a callback.
*/
class Client1 {
public:
    static void func(uint32_t v)
    {
        printf("static member callback: 0x%x\n", v);
    }
};

/**
* Dummy Client #2
* Uses an instance method as a callback
*/
class Client2 {
public:
    void func(uint32_t v) const
    {
        printf("instance member callback: 0x%x\n", v);
    }
};

/**
* Callback on a c function
*/
extern "C" {
static void c_client_callback(uint32_t v)
{
    printf("C function callback: 0x%x\n", v);
}
}

int main()
{
    /**
    * Examples using the basic driver
    * This is constructed with a specific value
    * Each callback receives this value
    */
    BasicDriver bd(0xDEADBEEF); //create basic driver instance
    Client2 c2;

    printf("Starting examples using the BasicDriver\n");
    // register a lambda function as a callback
    bd.register_callback(
        [](uint32_t v) {
            printf("lambda callback: 0x%x\n", v);
        });

    /**
    * std::bind is used to create a function object using
    * both the instance and method pointers.  This gives you the
    * callback into the specific object - very useful!
    *
    * std::bind keeps specific arguments at a fixed value (or a placeholder)
    * For our argument here, we keep as a placeholder.
    */
    bd.register_callback(
        std::bind(&Client2::func, &c2, std::placeholders::_1));

    // register a static class method as a callback
    bd.register_callback(&Client1::func);

    // register a C function pointer as a callback
    bd.register_callback(&c_client_callback);

    // call all the registered callbacks
    bd.callback();

    printf("End of examples using the BasicDriver\n");

    /**
    * Examples using the Event Driver.
    * Note that some callbacks will not be called -
    * their event type is different!
    */
    EventDriver ed(0xFEEDBEEF);

    printf("Beginning of examples using the EventDriver\n");

    // register a lambda function as a callback
    ed.register_callback(
        [](uint32_t v) {
            printf("lambda callback: 0x%x\n", v);
        }, VIDEO_START);

    // register client2 cb using std::bind
    ed.register_callback(
        std::bind(&Client2::func, &c2, std::placeholders::_1),
        VIDEO_STOP);

    // register a static class method as a callback
    ed.register_callback(&Client1::func, VIDEO_STOP);

    // register a C function pointer as a callback
    ed.register_callback(&c_client_callback, VIDEO_START);

    // call all the registered callbacks
    ed.callback();

    printf("End of examples using the EventDriver\n");

    /**
    * Examples using the Arg Driver.
    * Note that each callback registers for its own value
    */
    ArgDriver ad;

    printf("Beginning of examples using the ArgDriver\n");

    // register a lambda function as a callback
    ad.register_callback(
        [](uint32_t v) {
            printf("lambda callback: 0x%x\n", v);
        }, 0x0);

    // register client2 cb using std::bind
    ad.register_callback(
        std::bind(&Client2::func, &c2, std::placeholders::_1),
        0x1);

    // register a static class method as a callback
    ad.register_callback(&Client1::func, 0x2);

    // register a C function pointer as a callback
    ad.register_callback(&c_client_callback, 0x3);

    // call all the registered callbacks
    ad.callback();

    printf("End of examples using the ArgDriver\n");

    return 0;
}
