# pizza
Students eating pizza and studying something

This is a simple Dijkstra's semaphores / shared resources project.

student.c is a source for a  student process, which follows simple rules:
  1.It tries to grab a slice of pizza ( which is a semaphore with a specific number )
    A. If there is pizza, it eats it and studies while eating, going into sleep for some time, then goes back to step 1.
    B.If there is no pizza, and somebody else is still eating (other student is in sleep while having a slice), it is blocked until there is pizza
    C. If there is no pizza and this student is the last one awake, he 'calls' delivery service, which delivers pizza and goes back to step 1
    
delivery.c is a source for a delivery process, which passively waits for a call from a student, then 'delivers' (goes into sleep for a specified amount of time), and then updates pizza (sets the semaphore) with a specific number of slices 

manager.c is a source for a manager process, which manages all resources shared by objects of the first two types
