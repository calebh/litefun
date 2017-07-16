namespace litefun {
    // Smart pointer definitions
    template <class T>
    void swap_refs(T& a, T& b) {
        T c(a);
        a = b;
        b = c;
    }

    template <typename contained>
    class shared_ptr {
    public:
        shared_ptr() : ptr(nullptr), ref_count(nullptr) { }

        shared_ptr(contained * p)
            : ptr(p), ref_count(new int)
        {
            *ref_count = 0;
            inc_ref();
        }

        shared_ptr(const shared_ptr& rhs)
            : ptr(rhs.ptr), ref_count(rhs.ref_count)
        {
            inc_ref();
        }

        ~shared_ptr() {
            if (ref_count && 0 == dec_ref()) {
                if (ptr) {
                    delete ptr;
                }
                delete ref_count;
            }
        }

        void set(contained* p) {
            ptr = p;
        }

        contained *get() { return ptr; }
        const contained *get() const { return ptr; }

        void swap(shared_ptr& rhs) {
            swap_refs(ptr, rhs.ptr);
            swap_refs(ref_count, rhs.ref_count);
        }

        shared_ptr& operator=(const shared_ptr& rhs) {
            shared_ptr tmp(rhs);
            this->swap(tmp);
            return *this;
        }

        contained& operator*() {
            return *ptr;
        }

        contained *operator->() {
            return ptr;
        }

        bool operator==(shared_ptr& rhs) {
            return ptr == rhs.ptr;
        }

        bool operator!=(shared_ptr& rhs) { return !(rhs == *this); }

    private:
        void inc_ref() {
            if (ref_count) {
                ++(*ref_count);
            }
        }

        int dec_ref() {
            return --(*ref_count);
        }

        contained *ptr;
        int *ref_count;
    };

    // function definitions
    template<typename Func>
    struct func_filter
    {
        typedef Func type;
    };

    template<typename Result, typename ...Args>
    struct func_filter<Result(Args...)>
    {
        typedef Result(*type)(Args...);
    };

    template<typename Result, typename ...Args>
    struct abstract_function
    {
        virtual Result operator()(Args... args) = 0;
        virtual abstract_function *clone() const = 0;
        virtual ~abstract_function() = default;
    };

    template<typename Func, typename Result, typename ...Args>
    class concrete_function : public abstract_function<Result, Args...>
    {
        Func f;
    public:
        concrete_function(const Func &x)
            : f(x)
        {}

        Result operator()(Args... args) override {
            return f(args...);
        }

        concrete_function *clone() const override {
            return new concrete_function{ f };
        }
    };

    template<typename signature>
    class function;

    template<typename Result, typename ...Args>
    class function<Result(Args...)>
    {
        abstract_function<Result, Args...> *f;
    public:
        function()
            : f(nullptr)
        {}

        template<typename Func> function(const Func &x)
            : f(new concrete_function<typename func_filter<Func>::type, Result, Args...>(x))
        {}

        function(const function &rhs)
            : f(rhs.f ? rhs.f->clone() : nullptr)
        {}

        function &operator=(const function &rhs) {
            if ((&rhs != this) && (rhs.f))
            {
                auto *temp = rhs.f->clone();
                delete f;
                f = temp;
            }
            return *this;
        }

        template<typename Func>
        function &operator=(const Func &x) {
            auto *temp = new concrete_function<typename func_filter<Func>::type, Result, Args...>(x);
            delete f;
            f = temp;
            return *this;
        }

        Result operator()(Args... args) {
            if (f)
                return (*f)(args...);
            else
                return Result{};
        }

        ~function() {
            delete f;
        }
    };

    // smartfun definitions
    template<typename Result, typename ...Args>
    struct smart_abstract_function
    {
        virtual Result operator()(Args... args) = 0;
        virtual ~smart_abstract_function() = default;
    };

    template<typename Func, typename Result, typename ...Args>
    class smart_concrete_function : public smart_abstract_function<Result, Args...>
    {
        Func f;
    public:
        smart_concrete_function(const Func &x)
            : f(x)
        {}
        Result operator()(Args... args) override {
            return f(args...);
        }
    };

    template<typename signature>
    class smartfun;

    template<typename Result, typename ...Args>
    class smartfun<Result(Args...)>
    {
    public:
        shared_ptr<smart_abstract_function<Result, Args...>> f;
        smartfun()
            : f(nullptr) {
        }

        template<typename Func>
        smartfun(const Func &x)
            : f(new smart_concrete_function<typename func_filter<Func>::type, Result, Args...>(x)) {
        }

        smartfun(const smartfun &rhs)
            : f(rhs.f) {}

        smartfun &operator=(const smartfun &rhs) {
            if (&rhs != this) {
                f = rhs.f;
            }
            return *this;
        }

        template<typename Func>
        smartfun &operator=(const Func &rhs) {
            shared_ptr<smart_abstract_function<Result, Args...>> f2(new smart_concrete_function<typename func_filter<Func>::type, Result, Args...>(rhs));
            f = f2;
            return *this;
        }

        Result operator()(Args... args) {
            if (f.get() != nullptr) {
                return (*(f.get()))(args...);
            }
            else {
                return Result{};
            }
        }
    };
}