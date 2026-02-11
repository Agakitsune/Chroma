
#pragma once

namespace chroma {
    
    class MenuItem {
        public:
            MenuItem() noexcept = default;
            ~MenuItem() noexcept = default;

            virtual void menubar() noexcept = 0;
            virtual void action() noexcept {};
            virtual void display() noexcept {}; // Not everything has to display something
            virtual void shortcuts() noexcept {};
    };
}
